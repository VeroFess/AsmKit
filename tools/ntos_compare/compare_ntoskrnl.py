#!/usr/bin/env python3
import argparse
import json
import os
import random
import struct
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path


IMAGE_SCN_MEM_EXECUTE = 0x20000000


@dataclass(frozen=True)
class Section:
    name: str
    virtual_address: int
    virtual_size: int
    raw_offset: int
    raw_size: int
    characteristics: int


@dataclass(frozen=True)
class ExportFunction:
    name: str
    export_rva: int
    rva: int
    ordinal: int
    section: str
    file_offset: int
    max_size: int


class PEImage:
    def __init__(self, path: Path):
        self.path = path
        self.data = path.read_bytes()
        if len(self.data) < 0x100:
            raise ValueError(f"{path}: too small")
        if self.data[0:2] != b"MZ":
            raise ValueError(f"{path}: missing MZ header")
        self.pe_offset = self.u32(0x3c)
        if self.data[self.pe_offset:self.pe_offset + 4] != b"PE\0\0":
            raise ValueError(f"{path}: missing PE signature")
        file_header = self.pe_offset + 4
        (
            self.machine,
            self.number_of_sections,
            _time_date_stamp,
            _symtab,
            _symcount,
            self.optional_header_size,
            _characteristics,
        ) = struct.unpack_from("<HHIIIHH", self.data, file_header)
        opt = file_header + 20
        self.optional_header_offset = opt
        self.optional_magic = self.u16(opt)
        if self.optional_magic == 0x10B:
            self.image_base = self.u32(opt + 28)
            data_dir = opt + 96
        elif self.optional_magic == 0x20B:
            self.image_base = self.u64(opt + 24)
            data_dir = opt + 112
        else:
            raise ValueError(f"{path}: unsupported optional header magic 0x{self.optional_magic:x}")
        self.export_rva = self.u32(data_dir)
        self.export_size = self.u32(data_dir + 4)
        sec_off = opt + self.optional_header_size
        self.sections = []
        for index in range(self.number_of_sections):
            off = sec_off + index * 40
            raw_name = self.data[off:off + 8].split(b"\0", 1)[0]
            name = raw_name.decode("ascii", "replace")
            virtual_size, virtual_address, raw_size, raw_offset = struct.unpack_from("<IIII", self.data, off + 8)
            characteristics = self.u32(off + 36)
            self.sections.append(Section(name, virtual_address, virtual_size, raw_offset, raw_size, characteristics))

    def u16(self, off: int) -> int:
        return struct.unpack_from("<H", self.data, off)[0]

    def u32(self, off: int) -> int:
        return struct.unpack_from("<I", self.data, off)[0]

    def u64(self, off: int) -> int:
        return struct.unpack_from("<Q", self.data, off)[0]

    def section_for_rva(self, rva: int):
        for sec in self.sections:
            span = max(sec.virtual_size, sec.raw_size)
            if sec.virtual_address <= rva < sec.virtual_address + span:
                return sec
        return None

    def rva_to_offset(self, rva: int) -> int:
        sec = self.section_for_rva(rva)
        if sec is None:
            raise ValueError(f"{self.path}: RVA 0x{rva:x} is not mapped")
        delta = rva - sec.virtual_address
        if delta >= sec.raw_size:
            raise ValueError(f"{self.path}: RVA 0x{rva:x} is outside raw data")
        return sec.raw_offset + delta

    def read_c_string(self, rva: int) -> str:
        off = self.rva_to_offset(rva)
        end = self.data.find(b"\0", off)
        if end < 0:
            end = len(self.data)
        return self.data[off:end].decode("ascii", "replace")

    def bytes_at_rva(self, rva: int, size: int) -> bytes:
        off = self.rva_to_offset(rva)
        sec = self.section_for_rva(rva)
        assert sec is not None
        max_in_section = sec.raw_offset + sec.raw_size - off
        return self.data[off:off + min(size, max_in_section)]

    def exported_functions(self, max_window: int, thumb_entries: bool = False) -> list[ExportFunction]:
        if self.export_rva == 0:
            return []
        exp_off = self.rva_to_offset(self.export_rva)
        (
            _characteristics,
            _time_date_stamp,
            _major,
            _minor,
            _name,
            ordinal_base,
            function_count,
            name_count,
            functions_rva,
            names_rva,
            ordinals_rva,
        ) = struct.unpack_from("<IIHHIIIIIII", self.data, exp_off)
        functions_off = self.rva_to_offset(functions_rva)
        names_off = self.rva_to_offset(names_rva)
        ordinals_off = self.rva_to_offset(ordinals_rva)
        named = {}
        for index in range(name_count):
            name_rva = self.u32(names_off + index * 4)
            ordinal_index = self.u16(ordinals_off + index * 2)
            if ordinal_index >= function_count:
                continue
            named[ordinal_index] = self.read_c_string(name_rva)
        exports = []
        for index in range(function_count):
            rva = self.u32(functions_off + index * 4)
            if rva == 0:
                continue
            if self.export_rva <= rva < self.export_rva + self.export_size:
                continue
            code_rva = rva & ~1 if thumb_entries else rva
            sec = self.section_for_rva(code_rva)
            if sec is None or (sec.characteristics & IMAGE_SCN_MEM_EXECUTE) == 0:
                continue
            try:
                off = self.rva_to_offset(code_rva)
            except ValueError:
                continue
            name = named.get(index, f"ordinal_{ordinal_base + index}")
            sec_remaining = sec.raw_offset + sec.raw_size - off
            exports.append(ExportFunction(name, rva, code_rva, ordinal_base + index, sec.name, off, min(max_window, sec_remaining)))
        exports.sort(key=lambda item: (item.rva, item.name))
        return exports


def configure_capstone(arch: str, capstone_module):
    cs = capstone_module
    if arch == "x86":
        md = cs.Cs(cs.CS_ARCH_X86, cs.CS_MODE_32)
    elif arch == "amd64":
        md = cs.Cs(cs.CS_ARCH_X86, cs.CS_MODE_64)
    elif arch == "arm":
        md = cs.Cs(cs.CS_ARCH_ARM, cs.CS_MODE_THUMB)
    elif arch == "arm64":
        md = cs.Cs(cs.CS_ARCH_ARM64, getattr(cs, "CS_MODE_ARM", 0))
    else:
        raise ValueError(arch)
    md.detail = True
    return md


def asmkit_arch_mode(arch: str):
    if arch == "x86":
        return "x86", "32"
    if arch == "amd64":
        return "x86", "64"
    if arch == "arm":
        return "arm", "thumb"
    if arch == "arm64":
        return "aarch64", "aarch64"
    raise ValueError(arch)


X86_RARE = {
    "cpuid", "rdmsr", "wrmsr", "rdtsc", "rdtscp", "rdpid", "xgetbv", "xsetbv",
    "lgdt", "lidt", "lldt", "ltr", "sgdt", "sidt", "sldt", "str", "swapgs",
    "invd", "wbinvd", "invlpg", "sti", "cli", "hlt", "pause", "monitor", "mwait",
    "syscall", "sysret", "iret", "iretd", "iretq", "clflush", "clflushopt", "clwb",
    "fxsave", "fxrstor", "xsave", "xrstor", "xsaveopt", "xsavec", "xsaves", "xrstors",
    "vmcall", "vmlaunch", "vmresume", "vmxoff", "vmxon", "invept", "invvpid",
    "xbegin", "xend", "xtest", "cmpxchg8b", "cmpxchg16b",
}

A64_RARE = {
    "mrs", "msr", "sys", "sysl", "at", "tlbi", "ic", "dc", "cfp", "dvp", "cpp",
    "dsb", "dmb", "isb", "sev", "sevl", "wfe", "wfi", "yield", "eret", "eretaa", "eretab",
    "brk", "hlt", "svc", "smc", "hvc", "bti", "pacibsp", "paciasp", "autibsp", "autiasp",
    "braa", "brab", "blraa", "blrab", "retab", "retaa", "xpaclri", "irg", "addg", "subg",
    "ldar", "ldarb", "ldarh", "ldaxr", "ldaxrb", "ldaxrh", "ldxp", "ldxr", "ldxrb", "ldxrh",
    "stlr", "stlrb", "stlrh", "stlxr", "stlxrb", "stlxrh", "stxp", "stxr", "stxrb", "stxrh",
    "cas", "casa", "casal", "casl", "casb", "casab", "casalb", "caslb", "cash", "casah", "casalh", "caslh",
}

ARM_RARE = {
    "mrc", "mcr", "mrc2", "mcr2", "mrrc", "mcrr", "cps", "cpsid", "cpsie",
    "mrs", "msr", "dsb", "dmb", "isb", "sev", "wfe", "wfi", "yield", "nop",
    "svc", "hvc", "smc", "bkpt", "udf", "ldrex", "ldrexb", "ldrexh", "ldrexd",
    "strex", "strexb", "strexh", "strexd", "clrex", "pld", "pli", "rfe", "srs",
    "setend", "eret", "crc32b", "crc32h", "crc32w", "crc32cb", "crc32ch", "crc32cw",
    "vldmia", "vstmia", "vmrs", "vmsr",
}

NAME_KEYWORDS = [
    "hal", "ki", "ke", "mm", "ex", "ps", "se", "io", "po", "etw",
    "interrupt", "trap", "exception", "bugcheck", "processor", "context", "flush",
    "cache", "tlb", "spin", "interlocked", "lock", "register", "descriptor",
    "mtrr", "msr", "performance", "timer", "clock", "profile", "idle", "syscall",
]


def capstone_instructions(md, code: bytes, base: int, max_instructions: int):
    out = []
    for insn in md.disasm(code, base):
        out.append(insn)
        if len(out) >= max_instructions:
            break
    return out


def instruction_consumed_size(insns) -> int:
    if not insns:
        return 0
    first = insns[0].address
    last = insns[-1]
    return int(last.address - first + last.size)


def score_function(arch: str, func: ExportFunction, insns) -> tuple[int, dict]:
    lower_name = func.name.lower()
    score = 0
    reasons = []
    for keyword in NAME_KEYWORDS:
        if keyword in lower_name:
            score += 8
            reasons.append(f"name:{keyword}")
    rare_set = A64_RARE if arch == "arm64" else (ARM_RARE if arch == "arm" else X86_RARE)
    rare_hits = {}
    for insn in insns:
        mnemonic = insn.mnemonic.lower()
        bare = mnemonic.split()[-1]
        if bare in rare_set or mnemonic in rare_set:
            rare_hits[bare] = rare_hits.get(bare, 0) + 1
            score += 35
        if arch not in {"arm", "arm64"}:
            if bare.startswith("v") or "xmm" in insn.op_str or "ymm" in insn.op_str or "zmm" in insn.op_str:
                score += 4
            if mnemonic.startswith(("lock ", "rep ", "repe ", "repne ")):
                score += 12
        elif arch == "arm":
            if bare.startswith("v") or any(token in insn.op_str for token in ("s", "d", "q")):
                score += 2
            if "." in bare:
                score += 2
        else:
            if bare.startswith(("ld", "st")) and ("x" in insn.op_str or "sp" in insn.op_str):
                score += 2
            if "." in bare:
                score += 2
    if len(insns) >= 20:
        score += 5
    if rare_hits:
        reasons.extend(f"mnemonic:{name}x{count}" for name, count in sorted(rare_hits.items()))
    return score, {"score": score, "reasons": reasons[:16], "rare_hits": rare_hits}


X86_COND_ALIASES = {
    "z": "e",
    "nz": "ne",
    "nae": "b",
    "c": "b",
    "nb": "ae",
    "nc": "ae",
    "nbe": "a",
    "na": "be",
    "nge": "l",
    "ng": "le",
    "nle": "g",
    "nl": "ge",
    "pe": "p",
    "po": "np",
}


def normalize_x86_mnemonic(mnemonic: str) -> str:
    mnemonic = mnemonic.lower().strip()
    mnemonic = mnemonic.replace("repz ", "repe ").replace("repnz ", "repne ")
    for prefix in ("lock ", "rep ", "repe ", "repne ", "xacquire ", "xrelease "):
        if mnemonic.startswith(prefix):
            return prefix + normalize_x86_mnemonic(mnemonic[len(prefix):])
    if mnemonic in {"retn", "retf", "retq", "retl"}:
        return "ret"
    if mnemonic in {"iretd", "iretq"}:
        return "iret"
    if mnemonic == "movabs":
        return "mov"
    if mnemonic == "sal":
        return "shl"
    if mnemonic == "int 3":
        return "int3"
    for prefix in ("cmov", "set", "j"):
        if mnemonic.startswith(prefix) and len(mnemonic) > len(prefix):
            suffix = mnemonic[len(prefix):]
            suffix = X86_COND_ALIASES.get(suffix, suffix)
            return prefix + suffix
    return mnemonic


A64_ALIAS_GROUPS = [
    {"cmp", "subs"},
    {"cmn", "adds"},
    {"tst", "ands"},
    {"neg", "sub"},
    {"negs", "subs"},
    {"mvn", "orn"},
    {"mul", "madd"},
    {"mneg", "msub"},
    {"lsl", "ubfm"},
    {"lsr", "ubfm"},
    {"asr", "sbfm"},
    {"ubfx", "ubfm"},
    {"ubfiz", "ubfm"},
    {"sbfx", "sbfm"},
    {"sbfiz", "sbfm"},
    {"bfi", "bfm"},
    {"bfxil", "bfm"},
    {"sxtb", "sbfm"},
    {"sxth", "sbfm"},
    {"sxtw", "sbfm"},
    {"uxtb", "ubfm"},
    {"uxth", "ubfm"},
    {"nop", "hint"},
    {"yield", "hint"},
    {"wfe", "hint"},
    {"wfi", "hint"},
    {"sev", "hint"},
    {"sevl", "hint"},
    {"xpaclri", "hint"},
    {"ic", "sys"},
    {"dc", "sys"},
    {"at", "sys"},
    {"tlbi", "sys"},
    {"cfp", "sys"},
    {"dvp", "sys"},
    {"cpp", "sys"},
    {"umull", "umaddl"},
    {"smull", "smaddl"},
    {"cinc", "csinc"},
    {"cinv", "csinv"},
    {"cneg", "csneg"},
    {"csetm", "csinv"},
    {"mov", "mova"},
]


def normalize_a64_mnemonic(mnemonic: str) -> str:
    mnemonic = mnemonic.lower().strip()
    if mnemonic.startswith("b."):
        return mnemonic
    return mnemonic


def known_a64_alias(a: str, b: str) -> bool:
    if a == b:
        return True
    for group in A64_ALIAS_GROUPS:
        if a in group and b in group:
            return True
    if a == "mov" and b in {"orr", "add", "movz", "movn", "movk"}:
        return True
    if b == "mov" and a in {"orr", "add", "movz", "movn", "movk"}:
        return True
    if a == "mov" and b in {"umov", "smov"}:
        return True
    if b == "mov" and a in {"umov", "smov"}:
        return True
    if a.startswith("cset") and b.startswith("cs"):
        return True
    if b.startswith("cset") and a.startswith("cs"):
        return True
    return False


ARM_ALIAS_GROUPS = [
    {"mov", "movs"},
    {"lsl", "lsls"},
    {"lsr", "lsrs"},
    {"asr", "asrs"},
    {"add", "adds"},
    {"sub", "subs"},
    {"cmp", "subs"},
    {"cmn", "adds"},
    {"tst", "ands"},
    {"push", "stmdb"},
    {"pop", "ldm", "ldmia"},
    {"cdp2", "cx2a"},
    {"nop", "hint"},
    {"yield", "hint"},
    {"wfe", "hint"},
    {"wfi", "hint"},
    {"sev", "hint"},
]

ARM_SETFLAGS_BASES = {
    "adc", "add", "and", "asr", "bic", "eor", "lsl", "lsr",
    "mov", "mvn", "orr", "rsb", "rsc", "sbc", "sub",
}

ARM_CONDITION_SUFFIXES = (
    "eq", "ne", "hs", "cs", "lo", "cc", "mi", "pl",
    "vs", "vc", "hi", "ls", "ge", "lt", "gt", "le",
)


def normalize_arm_mnemonic(mnemonic: str) -> str:
    mnemonic = mnemonic.lower().strip()
    for suffix in (".w", ".n"):
        if mnemonic.endswith(suffix):
            mnemonic = mnemonic[:-len(suffix)]
    return mnemonic


def strip_arm_it_condition_suffix(mnemonic: str) -> str | None:
    for suffix in ARM_CONDITION_SUFFIXES:
        if mnemonic.endswith(suffix) and len(mnemonic) > len(suffix):
            base = mnemonic[:-len(suffix)]
            if base not in {"b", "bl"}:
                return base
    return None


def known_arm_alias(a: str, b: str) -> bool:
    if a == b:
        return True
    for group in ARM_ALIAS_GROUPS:
        if a in group and b in group:
            return True
    if a.endswith("s") and a[:-1] in ARM_SETFLAGS_BASES and a[:-1] == b:
        return True
    if b.endswith("s") and b[:-1] in ARM_SETFLAGS_BASES and b[:-1] == a:
        return True
    stripped_a = strip_arm_it_condition_suffix(a)
    if stripped_a is not None and stripped_a == b:
        return True
    stripped_b = strip_arm_it_condition_suffix(b)
    if stripped_b is not None and stripped_b == a:
        return True
    return False


def asmkit_mnemonic_from_text(arch: str, text: str) -> str:
    text = text.strip()
    if not text:
        return ""
    first = text.split(None, 2)
    if arch != "arm64" and first[0].lower() in {"lock", "rep", "repe", "repne", "repz", "repnz", "xacquire", "xrelease"} and len(first) >= 2:
        return f"{first[0]} {first[1].rstrip(',')}".lower()
    return first[0].rstrip(",").lower()


def normalize_mnemonic(arch: str, mnemonic: str) -> str:
    if arch == "arm64":
        return normalize_a64_mnemonic(mnemonic)
    if arch == "arm":
        return normalize_arm_mnemonic(mnemonic)
    return normalize_x86_mnemonic(mnemonic)


def mnemonics_equivalent(arch: str, cap: str, asmkit: str) -> tuple[bool, str]:
    cap_n = normalize_mnemonic(arch, cap)
    asm_n = normalize_mnemonic(arch, asmkit)
    if cap_n == asm_n:
        return True, "exact"
    if arch == "arm64" and known_a64_alias(cap_n, asm_n):
        return True, "known-aarch64-alias"
    if arch == "arm" and known_arm_alias(cap_n, asm_n):
        return True, "known-arm-alias"
    return False, "mismatch"


def parse_asmkit_output(text: str):
    instructions = []
    errors = []
    for line in text.splitlines():
        fields = line.rstrip("\n").split("\t")
        if not fields:
            continue
        if fields[0] == "I" and len(fields) >= 13:
            instructions.append({
                "address": int(fields[1], 16),
                "offset": int(fields[2]),
                "size": int(fields[3]),
                "bytes": fields[4],
                "id": int(fields[5]),
                "mnemonic_id": int(fields[6]),
                "class": int(fields[7]),
                "operand_count": int(fields[8]),
                "flags": fields[9],
                "attributes": fields[10],
                "text": fields[11],
                "operands": fields[12],
            })
        elif fields[0] == "E" and len(fields) >= 6:
            errors.append({
                "address": int(fields[1], 16),
                "offset": int(fields[2]),
                "status": fields[3],
                "code": int(fields[4]),
                "byte": fields[5],
            })
    return instructions, errors


def run_asmkit(helper: Path, arch: str, base: int, code: bytes):
    helper_arch, helper_mode = asmkit_arch_mode(arch)
    proc = subprocess.run(
        [str(helper), helper_arch, helper_mode, hex(base), code.hex()],
        cwd=str(Path.cwd()),
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        timeout=20,
    )
    instructions, errors = parse_asmkit_output(proc.stdout)
    return {
        "returncode": proc.returncode,
        "stdout": proc.stdout,
        "stderr": proc.stderr,
        "instructions": instructions,
        "errors": errors,
    }


def compare_function(helper: Path, arch: str, image: PEImage, md, func: ExportFunction, max_instructions: int):
    base = image.image_base + func.rva
    code = image.bytes_at_rva(func.rva, func.max_size)
    cap_insns = capstone_instructions(md, code, base, max_instructions)
    consumed = instruction_consumed_size(cap_insns)
    if consumed == 0:
        return {
            "name": func.name,
            "rva": func.rva,
            "status": "skipped-no-capstone-instructions",
            "instruction_count": 0,
            "critical_mismatches": [],
            "alias_differences": [],
            "detail_warnings": [],
        }
    code = code[:consumed]
    asm = run_asmkit(helper, arch, base, code)
    critical = []
    aliases = []
    warnings = []
    if asm["returncode"] != 0:
        critical.append({"kind": "helper-returncode", "returncode": asm["returncode"], "stderr": asm["stderr"][-400:]})
    for error in asm["errors"]:
        index = None
        for i, insn in enumerate(cap_insns):
            if int(insn.address - base) == error["offset"]:
                index = i
                break
        critical.append({
            "kind": "asmkit-decode-error",
            "offset": error["offset"],
            "status": error["status"],
            "byte": error["byte"],
            "capstone": None if index is None else {
                "mnemonic": cap_insns[index].mnemonic,
                "op_str": cap_insns[index].op_str,
                "size": cap_insns[index].size,
                "bytes": bytes(cap_insns[index].bytes).hex(),
            },
        })
    asm_by_offset = {item["offset"]: item for item in asm["instructions"]}
    for index, insn in enumerate(cap_insns):
        offset = int(insn.address - base)
        asm_item = asm_by_offset.get(offset)
        if asm_item is None:
            critical.append({
                "kind": "asmkit-missing-instruction",
                "index": index,
                "offset": offset,
                "capstone": f"{insn.mnemonic} {insn.op_str}".strip(),
                "bytes": bytes(insn.bytes).hex(),
            })
            continue
        if asm_item["size"] != insn.size:
            critical.append({
                "kind": "size",
                "index": index,
                "offset": offset,
                "capstone_size": insn.size,
                "asmkit_size": asm_item["size"],
                "capstone": f"{insn.mnemonic} {insn.op_str}".strip(),
                "asmkit": asm_item["text"],
                "bytes": bytes(insn.bytes).hex(),
            })
            continue
        asm_mnemonic = asmkit_mnemonic_from_text(arch, asm_item["text"])
        equivalent, reason = mnemonics_equivalent(arch, insn.mnemonic, asm_mnemonic)
        if not equivalent:
            critical.append({
                "kind": "mnemonic",
                "index": index,
                "offset": offset,
                "capstone_mnemonic": insn.mnemonic,
                "capstone_op_str": insn.op_str,
                "asmkit_text": asm_item["text"],
                "capstone_norm": normalize_mnemonic(arch, insn.mnemonic),
                "asmkit_norm": normalize_mnemonic(arch, asm_mnemonic),
                "bytes": bytes(insn.bytes).hex(),
            })
        elif reason != "exact":
            aliases.append({
                "index": index,
                "offset": offset,
                "capstone": f"{insn.mnemonic} {insn.op_str}".strip(),
                "asmkit": asm_item["text"],
                "reason": reason,
                "bytes": bytes(insn.bytes).hex(),
            })
        if asm_item["operand_count"] == 0 and insn.op_str:
            warnings.append({
                "kind": "asmkit-zero-operands-with-capstone-operands",
                "index": index,
                "offset": offset,
                "capstone": f"{insn.mnemonic} {insn.op_str}".strip(),
                "asmkit": asm_item["text"],
                "bytes": bytes(insn.bytes).hex(),
            })
    asm_offsets = {item["offset"] for item in asm["instructions"]}
    cap_offsets = {int(insn.address - base) for insn in cap_insns}
    for extra in sorted(asm_offsets - cap_offsets):
        asm_item = asm_by_offset[extra]
        critical.append({
            "kind": "asmkit-extra-instruction",
            "offset": extra,
            "asmkit": asm_item["text"],
            "bytes": asm_item["bytes"],
        })
    return {
        "name": func.name,
        "export_rva": func.export_rva,
        "rva": func.rva,
        "address": base,
        "section": func.section,
        "status": "ok" if not critical else "mismatch",
        "instruction_count": len(cap_insns),
        "byte_count": consumed,
        "critical_mismatches": critical,
        "alias_differences": aliases,
        "detail_warnings": warnings[:20],
    }


def select_samples(arch: str, image: PEImage, candidates: list[ExportFunction], md, random_count: int, target_count: int, seed: int, max_instructions: int):
    eligible = []
    scored = []
    for func in candidates:
        code = image.bytes_at_rva(func.rva, func.max_size)
        insns = capstone_instructions(md, code, image.image_base + func.rva, max_instructions)
        if not insns:
            continue
        eligible.append(func)
        score, detail = score_function(arch, func, insns)
        scored.append((score, func.name, func, detail))
    rng = random.Random(seed + {"x86": 17, "amd64": 31, "arm": 43, "arm64": 47}[arch])
    random_sample = rng.sample(eligible, min(random_count, len(eligible)))
    scored.sort(key=lambda item: (-item[0], item[1]))
    target_sample = [item[2] for item in scored[:min(target_count, len(scored))]]
    score_by_name = {item[2].name: item[3] for item in scored}
    union = {}
    for func in random_sample:
        union[func.name] = func
    for func in target_sample:
        union[func.name] = func
    return random_sample, target_sample, list(union.values()), score_by_name, len(eligible)


def summarize_results(results):
    summary = {
        "functions_compared": len(results),
        "functions_with_critical_mismatches": 0,
        "critical_mismatches": 0,
        "alias_differences": 0,
        "detail_warnings": 0,
        "instructions": 0,
    }
    for result in results:
        crit = len(result.get("critical_mismatches", []))
        if crit:
            summary["functions_with_critical_mismatches"] += 1
            summary["critical_mismatches"] += crit
        summary["alias_differences"] += len(result.get("alias_differences", []))
        summary["detail_warnings"] += len(result.get("detail_warnings", []))
        summary["instructions"] += result.get("instruction_count", 0)
    return summary


def import_capstone(pydeps: Path):
    if pydeps.exists():
        sys.path.insert(0, str(pydeps))
    import capstone
    return capstone


def main() -> int:
    parser = argparse.ArgumentParser(description="Compare AsmKit and Capstone on ntoskrnl export-entry samples.")
    parser.add_argument("--root", type=Path, default=Path.cwd())
    parser.add_argument("--ntos-dir", type=Path, default=Path("ntoskrnl"))
    parser.add_argument("--helper", type=Path, default=Path("build-asmkit-runtime-text/Debug/asmkit_decode_dump.exe"))
    parser.add_argument("--pydeps", type=Path, default=Path("build-asmkit-runtime-text/pydeps"))
    parser.add_argument("--report", type=Path, default=Path("build-asmkit-runtime-text/ntos_compare_report.json"))
    parser.add_argument("--max-window", type=int, default=384)
    parser.add_argument("--max-instructions", type=int, default=96)
    parser.add_argument("--random-count", type=int, default=100)
    parser.add_argument("--target-count", type=int, default=50)
    parser.add_argument("--seed", type=lambda s: int(s, 0), default=0xA5C0DE)
    args = parser.parse_args()

    root = args.root.resolve()
    ntos_dir = (root / args.ntos_dir).resolve()
    helper = (root / args.helper).resolve()
    pydeps = (root / args.pydeps).resolve()
    report = (root / args.report).resolve()
    if not helper.exists():
        raise SystemExit(f"helper not found: {helper}")
    capstone = import_capstone(pydeps)

    paths = {
        "x86": ntos_dir / "x86" / "ntoskrnl.exe",
        "amd64": ntos_dir / "amd64" / "ntoskrnl.exe",
        "arm": ntos_dir / "arm" / "ntoskrnl.exe",
        "arm64": ntos_dir / "arm64" / "ntoskrnl.exe",
    }
    final = {
        "capstone_version": getattr(capstone, "__version__", "unknown"),
        "max_window": args.max_window,
        "max_instructions": args.max_instructions,
        "random_count_requested": args.random_count,
        "target_count_requested": args.target_count,
        "seed": args.seed,
        "architectures": {},
    }
    overall_critical = 0
    for arch, path in paths.items():
        image = PEImage(path)
        md = configure_capstone(arch, capstone)
        candidates = image.exported_functions(args.max_window, thumb_entries=(arch == "arm"))
        random_sample, target_sample, union_sample, score_by_name, eligible_count = select_samples(
            arch,
            image,
            candidates,
            md,
            args.random_count,
            args.target_count,
            args.seed,
            args.max_instructions,
        )
        if len(random_sample) < args.random_count:
            raise SystemExit(f"{arch}: only {len(random_sample)} random-eligible functions")
        if len(target_sample) < args.target_count:
            raise SystemExit(f"{arch}: only {len(target_sample)} target-eligible functions")
        print(f"{arch}: exports={len(candidates)} eligible={eligible_count} random={len(random_sample)} targeted={len(target_sample)} unique={len(union_sample)}")
        results = []
        for index, func in enumerate(union_sample, 1):
            result = compare_function(helper, arch, image, md, func, args.max_instructions)
            results.append(result)
            if result["critical_mismatches"]:
                first = result["critical_mismatches"][0]
                print(f"{arch}: mismatch {func.name} +0x{first.get('offset', 0):x} {first.get('kind')}")
            elif index % 50 == 0:
                print(f"{arch}: compared {index}/{len(union_sample)}")
        summary = summarize_results(results)
        overall_critical += summary["critical_mismatches"]
        final["architectures"][arch] = {
            "path": str(path),
            "machine": image.machine,
            "image_base": image.image_base,
            "export_executable_candidates": len(candidates),
            "eligible_with_capstone_instructions": eligible_count,
            "random_functions": [func.name for func in random_sample],
            "targeted_functions": [
                {
                    "name": func.name,
                    "export_rva": func.export_rva,
                    "rva": func.rva,
                    "score": score_by_name.get(func.name, {}).get("score", 0),
                    "reasons": score_by_name.get(func.name, {}).get("reasons", []),
                    "rare_hits": score_by_name.get(func.name, {}).get("rare_hits", {}),
                }
                for func in target_sample
            ],
            "unique_functions_compared": [func.name for func in union_sample],
            "summary": summary,
            "results": results,
        }
        print(f"{arch}: summary {summary}")
    report.parent.mkdir(parents=True, exist_ok=True)
    report.write_text(json.dumps(final, indent=2), encoding="utf-8")
    print(f"report={report}")
    print(f"overall_critical_mismatches={overall_critical}")
    return 1 if overall_critical else 0


if __name__ == "__main__":
    raise SystemExit(main())
