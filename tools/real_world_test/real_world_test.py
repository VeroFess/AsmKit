#!/usr/bin/env python3
import argparse
import json
import os
import random
import re
import struct
import subprocess
import sys
from collections import Counter
from dataclasses import dataclass
from pathlib import Path


IMAGE_SCN_MEM_EXECUTE = 0x20000000
PT_LOAD = 1
PF_X = 1
SHT_SYMTAB = 2
SHT_DYNSYM = 11
SHN_UNDEF = 0
STT_FUNC = 2
STT_GNU_IFUNC = 10


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
    mode: str = ""
    source: str = ""


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
            max_size = sec_remaining if max_window <= 0 else min(max_window, sec_remaining)
            mode = "thumb" if thumb_entries else ""
            exports.append(ExportFunction(name, rva, code_rva, ordinal_base + index, sec.name, off, max_size, mode, str(self.path)))
        exports.sort(key=lambda item: (item.rva, item.name))
        return exports


@dataclass(frozen=True)
class ELFSegment:
    vaddr: int
    mem_size: int
    offset: int
    file_size: int
    flags: int


@dataclass(frozen=True)
class ELFSection:
    name: str
    section_type: int
    flags: int
    addr: int
    offset: int
    size: int
    link: int
    entsize: int


class ELFImage:
    def __init__(self, path: Path):
        self.path = path
        self.data = path.read_bytes()
        if len(self.data) < 0x40 or self.data[0:4] != b"\x7fELF":
            raise ValueError(f"{path}: missing ELF header")
        self.elf_class = self.data[4]
        self.endian = self.data[5]
        if self.endian != 1:
            raise ValueError(f"{path}: only little-endian ELF is supported")
        self.struct_prefix = "<"
        if self.elf_class == 1:
            header = struct.unpack_from("<16sHHIIIIIHHHHHH", self.data, 0)
            (
                _ident,
                self.elf_type,
                self.machine,
                _version,
                self.entry,
                self.phoff,
                self.shoff,
                _flags,
                _ehsize,
                self.phentsize,
                self.phnum,
                self.shentsize,
                self.shnum,
                self.shstrndx,
            ) = header
        elif self.elf_class == 2:
            header = struct.unpack_from("<16sHHIQQQIHHHHHH", self.data, 0)
            (
                _ident,
                self.elf_type,
                self.machine,
                _version,
                self.entry,
                self.phoff,
                self.shoff,
                _flags,
                _ehsize,
                self.phentsize,
                self.phnum,
                self.shentsize,
                self.shnum,
                self.shstrndx,
            ) = header
        else:
            raise ValueError(f"{path}: unsupported ELF class {self.elf_class}")
        self.image_base = 0
        self.segments = self._read_segments()
        self.sections = self._read_sections()

    def _read_segments(self) -> list[ELFSegment]:
        segments = []
        for index in range(self.phnum):
            off = self.phoff + index * self.phentsize
            if self.elf_class == 1:
                p_type, p_offset, p_vaddr, _p_paddr, p_filesz, p_memsz, p_flags, _p_align = struct.unpack_from("<IIIIIIII", self.data, off)
            else:
                p_type, p_flags, p_offset, p_vaddr, _p_paddr, p_filesz, p_memsz, _p_align = struct.unpack_from("<IIQQQQQQ", self.data, off)
            if p_type == PT_LOAD:
                segments.append(ELFSegment(p_vaddr, p_memsz, p_offset, p_filesz, p_flags))
        return segments

    def _section_name_table(self, raw_sections):
        if self.shstrndx >= len(raw_sections):
            return b""
        sh = raw_sections[self.shstrndx]
        return self.data[sh["offset"]:sh["offset"] + sh["size"]]

    @staticmethod
    def _read_string(blob: bytes, offset: int) -> str:
        if offset < 0 or offset >= len(blob):
            return ""
        end = blob.find(b"\0", offset)
        if end < 0:
            end = len(blob)
        return blob[offset:end].decode("utf-8", "replace")

    def _read_sections(self) -> list[ELFSection]:
        raw = []
        for index in range(self.shnum):
            off = self.shoff + index * self.shentsize
            if self.elf_class == 1:
                values = struct.unpack_from("<IIIIIIIIII", self.data, off)
                sh_name, sh_type, sh_flags, sh_addr, sh_offset, sh_size, sh_link, _sh_info, _sh_addralign, sh_entsize = values
            else:
                values = struct.unpack_from("<IIQQQQIIQQ", self.data, off)
                sh_name, sh_type, sh_flags, sh_addr, sh_offset, sh_size, sh_link, _sh_info, _sh_addralign, sh_entsize = values
            raw.append({
                "name_offset": sh_name,
                "type": sh_type,
                "flags": sh_flags,
                "addr": sh_addr,
                "offset": sh_offset,
                "size": sh_size,
                "link": sh_link,
                "entsize": sh_entsize,
            })
        names = self._section_name_table(raw)
        sections = []
        for item in raw:
            sections.append(ELFSection(
                self._read_string(names, item["name_offset"]),
                item["type"],
                item["flags"],
                item["addr"],
                item["offset"],
                item["size"],
                item["link"],
                item["entsize"],
            ))
        return sections

    def segment_for_va(self, va: int):
        for seg in self.segments:
            if seg.vaddr <= va < seg.vaddr + seg.mem_size:
                return seg
        return None

    def section_for_rva(self, va: int):
        for sec in self.sections:
            if sec.addr <= va < sec.addr + sec.size:
                return sec
        return None

    def rva_to_offset(self, va: int) -> int:
        seg = self.segment_for_va(va)
        if seg is None:
            raise ValueError(f"{self.path}: VA 0x{va:x} is not mapped")
        delta = va - seg.vaddr
        if delta >= seg.file_size:
            raise ValueError(f"{self.path}: VA 0x{va:x} is outside file-backed data")
        return seg.offset + delta

    def bytes_at_rva(self, va: int, size: int) -> bytes:
        off = self.rva_to_offset(va)
        seg = self.segment_for_va(va)
        assert seg is not None
        max_in_segment = seg.offset + seg.file_size - off
        return self.data[off:off + min(size, max_in_segment)]

    def _symbol_name_blob(self, section: ELFSection) -> bytes:
        if section.link >= len(self.sections):
            return b""
        strtab = self.sections[section.link]
        return self.data[strtab.offset:strtab.offset + strtab.size]

    def _read_symbol(self, offset: int):
        if self.elf_class == 1:
            st_name, st_value, st_size, st_info, _st_other, st_shndx = struct.unpack_from("<IIIBBH", self.data, offset)
        else:
            st_name, st_info, _st_other, st_shndx, st_value, st_size = struct.unpack_from("<IBBHQQ", self.data, offset)
        return st_name, st_info, st_shndx, st_value, st_size

    def exported_functions(self, max_window: int, thumb_entries: bool = False) -> list[ExportFunction]:
        del thumb_entries
        exports = []
        seen = set()
        for section in self.sections:
            if section.section_type not in {SHT_DYNSYM, SHT_SYMTAB} or section.entsize == 0:
                continue
            names = self._symbol_name_blob(section)
            count = section.size // section.entsize
            for index in range(count):
                off = section.offset + index * section.entsize
                st_name, st_info, st_shndx, st_value, st_size = self._read_symbol(off)
                sym_type = st_info & 0x0f
                if st_shndx == SHN_UNDEF or sym_type not in {STT_FUNC, STT_GNU_IFUNC}:
                    continue
                if st_value == 0:
                    continue
                name = self._read_string(names, st_name)
                if not name:
                    continue
                mode = ""
                code_va = st_value
                if self.machine == 40:
                    mode = "thumb" if (st_value & 1) != 0 else "a32"
                    code_va = st_value & ~1
                seg = self.segment_for_va(code_va)
                if seg is None or (seg.flags & PF_X) == 0:
                    continue
                try:
                    file_off = self.rva_to_offset(code_va)
                except ValueError:
                    continue
                sec = self.section_for_rva(code_va)
                section_name = sec.name if sec is not None else ""
                remaining = seg.offset + seg.file_size - file_off
                if st_size > 0:
                    remaining = min(remaining, st_size)
                max_size = remaining if max_window <= 0 else min(max_window, remaining)
                key = (code_va, name, mode)
                if key in seen or max_size <= 0:
                    continue
                seen.add(key)
                exports.append(ExportFunction(name, st_value, code_va, index, section_name, file_off, max_size, mode, str(self.path)))
        exports.sort(key=lambda item: (item.rva, item.name))
        return exports


def configure_capstone(arch: str, capstone_module, mode: str = ""):
    cs = capstone_module
    if arch == "x86":
        md = cs.Cs(cs.CS_ARCH_X86, cs.CS_MODE_32)
    elif arch == "amd64":
        md = cs.Cs(cs.CS_ARCH_X86, cs.CS_MODE_64)
    elif arch == "arm":
        capstone_mode = cs.CS_MODE_ARM if mode == "a32" else cs.CS_MODE_THUMB
        md = cs.Cs(cs.CS_ARCH_ARM, capstone_mode)
    elif arch == "arm64":
        md = cs.Cs(cs.CS_ARCH_ARM64, getattr(cs, "CS_MODE_ARM", 0))
    else:
        raise ValueError(arch)
    md.detail = True
    return md


def capstone_decoder(md_cache: dict, capstone_module, arch: str, mode: str = ""):
    key = (arch, mode)
    md = md_cache.get(key)
    if md is None:
        md = configure_capstone(arch, capstone_module, mode)
        md_cache[key] = md
    return md


def asmkit_arch_mode(arch: str, mode: str = ""):
    if arch == "x86":
        return "x86", "32"
    if arch == "amd64":
        return "x86", "64"
    if arch == "arm":
        return "arm", "a32" if mode == "a32" else "thumb"
    if arch == "arm64":
        return "aarch64", "aarch64"
    raise ValueError(arch)


def llvm_triple_args(arch: str, mode: str = "") -> list[str]:
    return llvm_triple_arg_sets(arch, mode)[0]


def llvm_triple_arg_sets(arch: str, mode: str = "") -> list[list[str]]:
    if arch == "x86":
        return [["-triple=i386-pc-windows-msvc", "--output-asm-variant=1"]]
    if arch == "amd64":
        return [["-triple=x86_64-pc-windows-msvc", "--output-asm-variant=1"]]
    if arch == "arm":
        if mode == "a32":
            return [
                ["--triple=armv8a-pc-windows-msvc"],
                ["--triple=armv7-pc-windows-msvc"],
            ]
        return [
            ["--triple=thumbv8a-pc-windows-msvc"],
            ["--triple=thumbv8m.main-pc-windows-msvc"],
            ["--triple=thumbv7-pc-windows-msvc"],
        ]
    if arch == "arm64":
        return [["-triple=aarch64-pc-windows-msvc", "--mattr=+all"]]
    raise ValueError(arch)


def llvm_instruction_comment_regex(arch: str):
    if arch in {"x86", "amd64"}:
        return re.compile(r"^(?P<text>.*?)\s+#\s+<MCInst\s+#(?P<id>\d+)\s+(?P<record>[A-Za-z0-9_.$]+)")
    if arch == "arm":
        return re.compile(r"^(?P<text>.*?)\s+@\s+<MCInst\s+#(?P<id>\d+)\s+(?P<record>[A-Za-z0-9_.$]+)")
    return re.compile(r"^(?P<text>.*?)\s+//\s+<MCInst\s+#(?P<id>\d+)\s+(?P<record>[A-Za-z0-9_.$]+)")


LLVM_MC_OPERAND_RE = re.compile(r"<MCOperand\s+([^:>]+)(?::([^>]*))?>")
X86_LLVM_PREFIX_RECORDS = {
    "LOCK_PREFIX",
    "REP_PREFIX",
    "REPNE_PREFIX",
}


def parse_llvm_mc_output(arch: str, stdout: str):
    record_re = llvm_instruction_comment_regex(arch)
    instructions = []
    current = None
    pending_prefixes = []
    for line in stdout.splitlines():
        stripped = line.strip()
        if not stripped:
            continue
        match = record_re.match(stripped)
        if match is not None:
            text = re.sub(r"\s+", " ", match.group("text").strip())
            mnemonic_text = text
            record = match.group("record")
            if arch in {"x86", "amd64"} and record in X86_LLVM_PREFIX_RECORDS:
                if text:
                    pending_prefixes.append(text)
                current = None
                continue
            if pending_prefixes:
                text = " ".join([*pending_prefixes, text])
                pending_prefixes = []
            mnemonic = mnemonic_text.split(None, 1)[0].rstrip(",").lower() if mnemonic_text else ""
            current = {
                "text": text,
                "mnemonic": mnemonic,
                "mc_id": int(match.group("id")),
                "llvm_name": record,
                "mc_operands": [],
            }
            instructions.append(current)
            continue
        operand_match = LLVM_MC_OPERAND_RE.search(stripped)
        if operand_match is not None and current is not None:
            current["mc_operands"].append({
                "kind": operand_match.group(1).strip(),
                "value": (operand_match.group(2) or "").strip(),
            })
    return instructions


def run_llvm_mc(llvm_mc: Path | None, arch: str, mode: str, code: bytes, expected_count: int | None = None):
    if llvm_mc is None:
        return None
    hex_text = " ".join(f"0x{byte:02x}" for byte in code)
    best = None
    best_score = None
    for args in llvm_triple_arg_sets(arch, mode):
        proc = subprocess.run(
            [str(llvm_mc), "--disassemble", "--show-inst", *args],
            input=hex_text,
            cwd=str(Path.cwd()),
            text=True,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            timeout=30,
        )
        instructions = parse_llvm_mc_output(arch, proc.stdout)
        warnings = [line for line in proc.stderr.splitlines() if "warning:" in line.lower() or "error:" in line.lower()]
        fatal_warning_count = sum(1 for line in warnings if "potentially undefined instruction encoding" not in line)
        count_delta = 0 if expected_count is None else abs(len(instructions) - expected_count)
        score = (0 if proc.returncode == 0 else 1, fatal_warning_count, count_delta, -len(instructions))
        candidate = {
            "returncode": proc.returncode,
            "stdout": proc.stdout,
            "stderr": proc.stderr,
            "instructions": instructions,
            "warnings": warnings,
            "args": args,
        }
        if best_score is None or score < best_score:
            best = candidate
            best_score = score
    return best


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
            normalized_tail = normalize_x86_mnemonic(mnemonic[len(prefix):])
            if normalized_tail == "ret" and prefix == "repe ":
                return "rep ret"
            return prefix + normalized_tail
    if mnemonic in {"retn", "retf", "retq", "retl"}:
        return "ret"
    if mnemonic in {"iretd", "iretq"}:
        return "iret"
    if mnemonic == "movabs":
        return "mov"
    if mnemonic == "sal":
        return "shl"
    if mnemonic == "fldt":
        return "fld"
    if mnemonic == "fiadds":
        return "fiadd"
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
    {"bxns", "bx"},
    {"blxns", "blx"},
    {"tst", "ands"},
    {"neg", "sub"},
    {"negs", "subs"},
    {"ngc", "sbc"},
    {"ngcs", "sbcs"},
    {"mvn", "orn"},
    {"mvn", "not"},
    {"not", "eor"},
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
    {"csdb", "hint"},
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
    {"smstart", "msr"},
    {"smstop", "msr"},
    {"umull", "umaddl"},
    {"smull", "smaddl"},
    {"cinc", "csinc"},
    {"cinv", "csinv"},
    {"cneg", "csneg"},
    {"csetm", "csinv"},
    {"mov", "mova"},
    {"mov", "ins"},
    {"mov", "dup"},
    {"mov", "sel"},
    {"ror", "extr"},
]


def normalize_a64_mnemonic(mnemonic: str) -> str:
    mnemonic = mnemonic.lower().strip()
    if mnemonic.startswith("b."):
        return mnemonic
    return mnemonic


def known_a64_alias(a: str, b: str) -> bool:
    if a == b:
        return True
    if (a.startswith("bc.") and b == "bc") or (b.startswith("bc.") and a == "bc"):
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
    {"ror", "rors"},
    {"add", "adds"},
    {"addw", "subw", "adr"},
    {"sub", "subs"},
    {"cmp", "subs"},
    {"cmn", "adds"},
    {"tst", "ands"},
    {"lsl", "lsr", "asr", "ror", "mov"},
    {"push", "stmdb"},
    {"pop", "ldr", "ldm", "ldmia"},
    {"vpush", "vstmdb"},
    {"vpop", "vldm", "vldmia"},
    {"bxns", "bx"},
    {"blxns", "blx"},
    {"cdp2", "cx2a"},
    {"cdp2", "cx3a"},
    {"cdp2", "vpt.f16"},
    {"nop", "hint"},
    {"yield", "hint"},
    {"wfe", "hint"},
    {"wfi", "hint"},
    {"sev", "hint"},
]

ARM_SETFLAGS_BASES = {
    "adc", "add", "and", "asr", "bic", "eor", "lsl", "lsr", "ror",
    "mla", "mov", "mul", "mvn", "orn", "orr", "rsb", "rsc", "sbc",
    "smlal", "smull", "sub", "umlal", "umull",
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
    dot_suffix = ""
    head = mnemonic
    if "." in mnemonic:
        head, tail = mnemonic.split(".", 1)
        dot_suffix = "." + tail
    for suffix in ARM_CONDITION_SUFFIXES:
        if head.endswith(suffix) and len(head) > len(suffix):
            base = head[:-len(suffix)]
            if base not in {"b", "bl"} and (len(base) >= 3 or base in {"bx"}):
                return base + dot_suffix
    return None


def arm_it_following_instruction_count(mnemonic: str) -> int:
    mnemonic = normalize_arm_mnemonic(mnemonic)
    if re.fullmatch(r"it[te]{0,3}", mnemonic) is None:
        return 0
    return len(mnemonic) - 1


def known_arm_alias(a: str, b: str) -> bool:
    if a == b:
        return True
    a_base = strip_arm_it_condition_suffix(a) or a
    b_base = strip_arm_it_condition_suffix(b) or b
    a_no_flags = a_base[:-1] if a_base.endswith("s") and a_base[:-1] in ARM_SETFLAGS_BASES else a_base
    b_no_flags = b_base[:-1] if b_base.endswith("s") and b_base[:-1] in ARM_SETFLAGS_BASES else b_base
    if a_no_flags == b_no_flags:
        return True
    for group in ARM_ALIAS_GROUPS:
        if a in group and b in group:
            return True
        if a_no_flags in group and b_no_flags in group:
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


def x86_notrack_prefix_equivalent(arch: str, cap: str, asmkit: str, asm_item: dict) -> bool:
    if arch not in {"x86", "amd64"}:
        return False
    cap_n = normalize_x86_mnemonic(cap)
    if not cap_n.startswith("notrack "):
        return False
    if not asm_item.get("bytes", "").lower().startswith("3e"):
        return False
    return cap_n[len("notrack "):] == normalize_x86_mnemonic(asmkit)


ASMKIT_INST_FLAGS = {
    "pc-relative": 1 << 0,
    "direct": 1 << 1,
    "conditional": 1 << 2,
    "call": 1 << 3,
    "return": 1 << 4,
    "terminator": 1 << 5,
    "literal": 1 << 6,
    "state-switch": 1 << 7,
    "pair-prefix": 1 << 8,
    "unsupported-reloc": 1 << 9,
    "x86-lock": 1 << 10,
    "x86-rep": 1 << 11,
    "x86-repne": 1 << 12,
}

ASMKIT_INSTRUCTION_ATTRS = {
    "branch": 1 << 0,
    "conditional": 1 << 1,
    "indirect": 1 << 2,
    "call": 1 << 3,
    "return": 1 << 4,
    "terminator": 1 << 5,
    "load": 1 << 6,
    "store": 1 << 7,
    "alu": 1 << 8,
    "pc-relative": 1 << 9,
    "pseudo": 1 << 10,
    "atomic": 1 << 11,
    "acquire-release": 1 << 12,
    "pair": 1 << 13,
    "x86-nf": 1 << 14,
    "x86-sae": 1 << 15,
}

ASMKIT_OP_FLAGS = {
    "explicit": 1 << 0,
    "read": 1 << 1,
    "write": 1 << 2,
    "relative": 1 << 3,
    "memory": 1 << 4,
    "signed": 1 << 5,
    "x86-zeroing": 1 << 6,
    "x86-broadcast": 1 << 7,
    "aarch64-extended-reg": 1 << 8,
    "register-list": 1 << 9,
    "vector-list": 1 << 10,
    "arm-negative-offset": 1 << 11,
    "register-pair": 1 << 12,
}

ASMKIT_REG_EFFECT_FLAGS = {
    "read": 1 << 0,
    "write": 1 << 1,
    "implicit": 1 << 2,
}

ASMKIT_INST_CLASS_NAMES = {
    0: "other",
    1: "nop",
    2: "hint",
    3: "landing-pad",
    4: "direct-branch",
    5: "indirect-branch",
    6: "cond-branch",
    7: "direct-call",
    8: "indirect-call",
    9: "return",
    10: "load",
    11: "store",
    12: "alu",
    13: "system",
    14: "barrier",
    15: "pac",
    16: "bti",
    17: "cet-endbr",
    18: "arm-it",
    19: "wasm-block",
    20: "wasm-loop",
    21: "wasm-if",
    22: "wasm-else",
    23: "wasm-end",
    24: "bpf-pseudo",
}

A64_CONDITIONS = {
    "eq": 0, "ne": 1, "hs": 2, "cs": 2, "lo": 3, "cc": 3,
    "mi": 4, "pl": 5, "vs": 6, "vc": 7, "hi": 8, "ls": 9,
    "ge": 10, "lt": 11, "gt": 12, "le": 13, "al": 14, "nv": 15,
}

A64_CONDITION_OPERAND_MNEMONICS = {
    "csel", "csinc", "csinv", "csneg", "ccmp", "ccmn", "fcsel", "fccmp", "fccmpe",
}

A64_FIXED_ZERO_IMM_MNEMONICS = {
    "cmeq", "cmge", "cmgt", "cmle", "cmlt",
    "fcmeq", "fcmge", "fcmgt", "fcmle", "fcmlt",
}

ARM_DEFAULT_ZERO_IMM_MNEMONICS = {
    "pkhbt", "pkhtb",
    "sxtab", "sxtab16", "sxtah", "sxtb", "sxtb16", "sxth",
    "uxtab", "uxtab16", "uxtah", "uxtb", "uxtb16", "uxth",
}

BARRIER_OPTION_IMMS = {
    "oshld": 1, "oshst": 2, "osh": 3,
    "nshld": 5, "nshst": 6, "nsh": 7,
    "ishld": 9, "ishst": 10, "ish": 11,
    "ld": 13, "st": 14, "sy": 15,
}

X86_PREFIX_WORDS = ("lock ", "rep ", "repe ", "repne ", "repz ", "repnz ", "xacquire ", "xrelease ")

X86_STRING_MNEMONICS = {
    "ins", "insb", "insw", "insd",
    "outs", "outsb", "outsw", "outsd",
    "lods", "lodsb", "lodsw", "lodsd", "lodsq",
    "stos", "stosb", "stosw", "stosd", "stosq",
    "scas", "scasb", "scasw", "scasd", "scasq",
    "cmps", "cmpsb", "cmpsw", "cmpsd", "cmpsq",
    "movs", "movsb", "movsw", "movsd", "movsq",
}

ARM_REG_ALIASES = {
    "a1": "r0", "a2": "r1", "a3": "r2", "a4": "r3",
    "v1": "r4", "v2": "r5", "v3": "r6", "v4": "r7",
    "v5": "r8", "sb": "r9", "sl": "r10", "fp": "r11",
    "ip": "r12",
}

A64_REG_ALIASES = {
    "fp": "x29",
    "lr": "x30",
}


def parse_int(text: str) -> int:
    return int(text, 0)


def sign_extend(value: int, width: int) -> int:
    if width <= 0 or width > 64:
        return value
    mask = (1 << width) - 1
    value &= mask
    sign = 1 << (width - 1)
    return (value ^ sign) - sign


def possible_signed_values(value: int, width: int) -> set[int]:
    values = {value}
    if 0 < width <= 64:
        values.add(sign_extend(value, width))
        if value >= 0:
            values.add(value & ((1 << width) - 1))
    return values


def values_equivalent(left: int, right: int, left_width: int = 0, right_width: int = 0) -> bool:
    left_values = possible_signed_values(left, left_width)
    right_values = possible_signed_values(right, right_width)
    return not left_values.isdisjoint(right_values)


def normalize_register_name(arch: str, name: str | None) -> str:
    if name is None:
        return ""
    out = name.lower().strip()
    if out in {"", "-", "invalid"}:
        return ""
    if out.startswith("%"):
        out = out[1:]
    out = out.replace("st(", "st").replace(")", "")
    if arch in {"x86", "amd64"} and out == "riz":
        return ""
    if arch in {"x86", "amd64"} and out == "st":
        out = "st0"
    if arch in {"x86", "amd64"} and out in {"eflags", "rflags"}:
        out = "flags"
    if arch == "arm":
        out = ARM_REG_ALIASES.get(out, out)
    elif arch == "arm64":
        out = A64_REG_ALIASES.get(out, out)
        if len(out) > 1 and out[0] == "v" and out[1:].isdigit():
            out = "q" + out[1:]
    return out


def x86_strip_prefix_words(mnemonic: str) -> str:
    out = mnemonic.lower().strip()
    changed = True
    while changed:
        changed = False
        for prefix in X86_PREFIX_WORDS:
            if out.startswith(prefix):
                out = out[len(prefix):]
                changed = True
                break
    return out


def arm_reg_name_from_list_bit(bit: int) -> str:
    if bit <= 12:
        return f"r{bit}"
    if bit == 13:
        return "sp"
    if bit == 14:
        return "lr"
    if bit == 15:
        return "pc"
    return f"r{bit}"


def a64_vector_list_names(first_reg: str, shape: int) -> tuple[str, ...]:
    if len(first_reg) < 2 or not first_reg[1:].isdigit():
        return (first_reg,) if first_reg else ()
    count = shape & 0xff
    if count <= 0 or count > 4:
        count = 1
    prefix = first_reg[0]
    start = int(first_reg[1:])
    return tuple(f"{prefix}{(start + offset) % 32}" for offset in range(count))


def arm_vector_list_names(first_reg: str, shape: int) -> tuple[str, ...]:
    if len(first_reg) < 2 or not first_reg[1:].isdigit():
        return (first_reg,) if first_reg else ()
    count = shape & 0xff
    if count <= 0 or count > 32:
        count = 1
    prefix = first_reg[0]
    start = int(first_reg[1:])
    return tuple(f"{prefix}{start + offset}" for offset in range(count))


def a64_capstone_vector_reg_name(name: str, op_str: str) -> str:
    lower = name.lower()
    if len(lower) < 2 or lower[0] != "v" or not lower[1:].isdigit():
        return name
    match = re.search(rf"\b{re.escape(lower)}\.([0-9]+[bhsdq])\b", op_str.lower())
    if match is None:
        return name
    arrangement = match.group(1)
    if arrangement in {"8b", "4h", "2s", "1d"}:
        return "d" + lower[1:]
    if arrangement in {"16b", "8h", "4s", "2d", "1q"}:
        return "q" + lower[1:]
    return name


def normalize_x86_absolute_target(arch: str, value: int, address: int, size: int) -> int:
    if arch in {"x86", "amd64"} and value < 0x100000000:
        return value
    return value


def parse_asmkit_operands(text: str):
    operands = []
    if not text:
        return operands
    for part in text.split(";"):
        fields = part.split(",")
        if len(fields) >= 26:
            operands.append({
                "kind": fields[0],
                "width": parse_int(fields[1]),
                "flags": parse_int(fields[2]),
                "reg": parse_int(fields[3]),
                "reg_name": fields[4],
                "shift_reg": parse_int(fields[5]),
                "shift_reg_name": fields[6],
                "imm": parse_int(fields[7]),
                "abs_target": parse_int(fields[8]),
                "mem_base": parse_int(fields[9]),
                "mem_base_name": fields[10],
                "mem_index": parse_int(fields[11]),
                "mem_index_name": fields[12],
                "mem_scale": parse_int(fields[13]),
                "mem_disp": parse_int(fields[14]),
                "mem_address_width": parse_int(fields[15]),
                "mem_segment": parse_int(fields[16]),
                "mem_segment_name": fields[17],
                "shift_kind": parse_int(fields[18]),
                "shift_amount": parse_int(fields[19]),
                "extend_kind": parse_int(fields[20]),
                "extend_amount": parse_int(fields[21]),
                "role": parse_int(fields[22]),
                "encoding": parse_int(fields[23]),
                "info_width": parse_int(fields[24]),
                "info_flags": parse_int(fields[25]),
            })
        elif len(fields) >= 11:
            operands.append({
                "kind": fields[0],
                "width": parse_int(fields[1]),
                "flags": parse_int(fields[2]),
                "reg": parse_int(fields[3]),
                "reg_name": "",
                "shift_reg": 0,
                "shift_reg_name": "",
                "imm": parse_int(fields[4]),
                "abs_target": 0,
                "mem_base": parse_int(fields[5]),
                "mem_base_name": "",
                "mem_index": parse_int(fields[6]),
                "mem_index_name": "",
                "mem_scale": parse_int(fields[7]),
                "mem_disp": parse_int(fields[8]),
                "mem_address_width": parse_int(fields[9]),
                "mem_segment": parse_int(fields[10]),
                "mem_segment_name": "",
                "shift_kind": 0,
                "shift_amount": 0,
                "extend_kind": 0,
                "extend_amount": 0,
                "role": 0,
                "encoding": 0,
                "info_width": 0,
                "info_flags": 0,
            })
    return operands


def parse_asmkit_effects(text: str, arch: str):
    effects = []
    if not text:
        return effects
    for part in text.split(";"):
        fields = part.split(",")
        if len(fields) < 3:
            continue
        effects.append({
            "reg": parse_int(fields[0]),
            "reg_name": normalize_register_name(arch, fields[1]),
            "flags": parse_int(fields[2]),
        })
    return effects


def capstone_operand_constants(arch: str):
    import capstone
    if arch in {"x86", "amd64"}:
        return capstone.x86.X86_OP_REG, capstone.x86.X86_OP_IMM, capstone.x86.X86_OP_MEM
    if arch == "arm":
        return capstone.arm.ARM_OP_REG, capstone.arm.ARM_OP_IMM, capstone.arm.ARM_OP_MEM
    if arch == "arm64":
        return capstone.arm64.ARM64_OP_REG, capstone.arm64.ARM64_OP_IMM, capstone.arm64.ARM64_OP_MEM
    return 1, 2, 3


def capstone_insn_semantics(arch: str, insn) -> dict:
    import capstone
    mnemonic = normalize_mnemonic(arch, insn.mnemonic)
    arm_base_mnemonic = strip_arm_it_condition_suffix(mnemonic) or mnemonic if arch == "arm" else mnemonic
    groups = set(insn.groups)
    branch = getattr(capstone, "CS_GRP_JUMP", -1) in groups
    call = getattr(capstone, "CS_GRP_CALL", -1) in groups
    ret = getattr(capstone, "CS_GRP_RET", -1) in groups
    if arch in {"x86", "amd64"}:
        branch = branch or mnemonic.startswith("j") or mnemonic in {"loop", "loope", "loopne"}
        call = call or mnemonic == "call"
        ret = ret or mnemonic in {"ret", "iret"}
    elif arch == "arm":
        call = call or arm_base_mnemonic in {"bl", "blx"}
        branch = branch or arm_base_mnemonic in {"b", "bx", "cbz", "cbnz", "tbb", "tbh"}
    elif arch == "arm64":
        call = call or mnemonic in {"bl", "blr", "blraa", "blrab"}
        ret = ret or mnemonic in {"ret", "retaa", "retab", "eret", "eretaa", "eretab"}
        branch = branch or mnemonic.startswith("b.") or mnemonic in {"b", "br", "cbz", "cbnz", "tbz", "tbnz", "bc"} or mnemonic.startswith("bc.")
    conditional = False
    if branch:
        if arch in {"x86", "amd64"}:
            conditional = mnemonic.startswith("j") and mnemonic != "jmp"
            conditional = conditional or mnemonic in {"loop", "loope", "loopne", "jrcxz", "jecxz", "jcxz"}
        elif arch == "arm":
            conditional = arm_base_mnemonic in {"cbz", "cbnz"} or strip_arm_it_condition_suffix(mnemonic) is not None
        elif arch == "arm64":
            conditional = mnemonic.startswith("b.") or mnemonic in {"cbz", "cbnz", "tbz", "tbnz"} or mnemonic.startswith("bc.")
    indirect = False
    if arch in {"x86", "amd64"} and (branch or call):
        indirect = bool(insn.operands and getattr(insn.operands[0], "type", 0) != capstone_operand_constants(arch)[1])
    elif arch == "arm":
        indirect = arm_base_mnemonic in {"tbb", "tbh"} or (arm_base_mnemonic in {"bx", "blx"} and not any(getattr(op, "type", 0) == capstone_operand_constants(arch)[1] for op in insn.operands))
    elif arch == "arm64":
        indirect = mnemonic in {"br", "blr", "ret", "retaa", "retab"}
    return {
        "branch": branch and not call,
        "call": call,
        "return": ret,
        "conditional": conditional,
        "indirect": indirect,
        "direct": (branch or call) and not indirect,
        "terminator": branch or call or ret,
    }


def capstone_operands(arch: str, insn):
    import capstone
    reg_type, imm_type, mem_type = capstone_operand_constants(arch)
    semantics = capstone_insn_semantics(arch, insn)
    mnemonic = normalize_mnemonic(arch, insn.mnemonic)
    has_mem_operand = any(getattr(op, "type", 0) == mem_type for op in getattr(insn, "operands", []))
    out = []
    for index, op in enumerate(getattr(insn, "operands", [])):
        access = getattr(op, "access", 0)
        width = getattr(op, "size", 0) or 0
        if op.type == reg_type:
            reg_name = insn.reg_name(op.reg)
            if arch == "arm64":
                reg_name = a64_capstone_vector_reg_name(reg_name, insn.op_str)
            out.append({
                "kind": "reg",
                "reg": normalize_register_name(arch, reg_name),
                "width": width * 8,
                "access": access,
                "shift_kind": getattr(getattr(op, "shift", None), "type", 0),
                "shift_amount": getattr(getattr(op, "shift", None), "value", 0),
                "extend_kind": getattr(op, "ext", 0),
                "vector_index": getattr(op, "vector_index", -1),
            })
        elif op.type == imm_type or (arch == "arm" and op.type in {capstone.arm.ARM_OP_CIMM, capstone.arm.ARM_OP_PIMM, capstone.arm.ARM_OP_SETEND, capstone.arm.ARM_OP_SYSREG}):
            kind = "target" if (semantics["branch"] or semantics["call"]) else "imm"
            imm_value = int(op.imm)
            if arch == "arm64" and mnemonic in {"add", "adds", "sub", "subs", "cmp", "cmn"} and kind == "imm":
                shift = capstone_lsl_shift_amount(insn.op_str)
                if shift > 0:
                    imm_value <<= shift
            if arch == "arm64" and mnemonic in {"adr", "adrp"}:
                kind = "target"
            if arch == "arm64" and mnemonic in {"tbz", "tbnz"}:
                kind = "target" if index == 2 else "imm"
            if arch == "arm" and mnemonic == "adr":
                kind = "target"
                imm_value = ((int(insn.address) + 4) & ~3) + imm_value
            if arch in {"arm", "arm64"} and mnemonic.startswith(("ldr", "ldrsw", "prfm")) and not has_mem_operand:
                kind = "target"
            elif arch in {"arm", "arm64"} and mnemonic.startswith(("ld", "st")):
                kind = "mem-disp"
            out.append({
                "kind": kind,
                "imm": imm_value,
                "width": width * 8,
                "access": access,
            })
        elif op.type == mem_type:
            mem = op.mem
            scale = capstone_memory_scale(arch, insn, op, getattr(mem, "scale", 0) or 1)
            out.append({
                "kind": "mem",
                "base": normalize_register_name(arch, insn.reg_name(mem.base)) if getattr(mem, "base", 0) else "",
                "index": normalize_register_name(arch, insn.reg_name(mem.index)) if getattr(mem, "index", 0) else "",
                "scale": scale,
                "disp": int(getattr(mem, "disp", 0)),
                "segment": normalize_register_name(arch, insn.reg_name(mem.segment)) if hasattr(mem, "segment") and getattr(mem, "segment", 0) else "",
                "width": width * 8,
                "access": access,
            })
        else:
            out.append({"kind": f"capstone-op-{op.type}", "access": access, "width": width * 8})
    return out


def asmkit_register_list_names(arch: str, value: int) -> tuple[str, ...]:
    if arch == "arm":
        return tuple(arm_reg_name_from_list_bit(bit) for bit in range(16) if (value & (1 << bit)) != 0)
    return ()


def asmkit_operands(arch: str, asm_item):
    out = []
    asm_mnemonic = asmkit_mnemonic_from_text(arch, asm_item["text"])
    has_asm_memory_operand = any(op["kind"] == "mem" for op in asm_item["operands_parsed"])
    seen_a64_vector_list = False
    saw_a64_vector_postindex_xzr = False
    saw_a64_vector_lane_imm = False
    for op in asm_item["operands_parsed"]:
        flags = op["flags"]
        info_flags = op.get("info_flags", 0)
        access = 0
        if (flags | info_flags) & ASMKIT_OP_FLAGS["read"]:
            access |= 1
        if (flags | info_flags) & ASMKIT_OP_FLAGS["write"]:
            access |= 2
        if op["kind"] == "reg" and (flags & ASMKIT_OP_FLAGS["register-list"]) != 0:
            out.append({
                "kind": "reglist",
                "regs": asmkit_register_list_names(arch, op["reg"]),
                "width": op["width"],
                "access": access,
                "raw": op,
            })
        elif op["kind"] == "reg" and (flags & ASMKIT_OP_FLAGS["vector-list"]) != 0:
            reg_name = normalize_register_name(arch, op["reg_name"])
            if arch == "arm64":
                seen_a64_vector_list = True
            out.append({
                "kind": "reglist",
                "regs": (
                    a64_vector_list_names(reg_name, int(op.get("imm", 0)))
                    if arch == "arm64"
                    else arm_vector_list_names(reg_name, int(op.get("imm", 0)))
                ),
                "width": op["info_width"] or op["width"],
                "access": access,
                "raw": op,
            })
        elif op["kind"] == "reg":
            role = op.get("role", 0)
            encoding = op.get("encoding", 0)
            reg_name = normalize_register_name(arch, op["reg_name"])
            if not reg_name and arch == "arm" and 1 <= op["reg"] <= 16:
                reg_name = arm_reg_name_from_list_bit(op["reg"] - 1)
            if arch == "arm64" and seen_a64_vector_list and asm_mnemonic.startswith(("ld", "st")) and reg_name == "xzr":
                saw_a64_vector_postindex_xzr = True
                continue
            kind = "reg"
            if arch in {"arm", "arm64"} and op.get("kind") == "mem":
                kind = "mem-base"
            is_readwrite = ((op.get("info_flags", 0) | flags) & (ASMKIT_OP_FLAGS["read"] | ASMKIT_OP_FLAGS["write"])) == (ASMKIT_OP_FLAGS["read"] | ASMKIT_OP_FLAGS["write"])
            if (
                arch in {"arm", "arm64"}
                and (asm_mnemonic.startswith(("ld", "st")) or asm_mnemonic == "prfm")
                and is_readwrite
                and (arch == "arm64" or has_asm_memory_operand)
            ):
                kind = "mem-base"
            out.append({
                "kind": kind,
                "reg": reg_name,
                "width": op["info_width"] or op["width"],
                "access": access,
                "shift_kind": op["shift_kind"],
                "shift_amount": op["shift_amount"],
                "shift_reg": normalize_register_name(arch, op["shift_reg_name"]),
                "extend_kind": op["extend_kind"],
                "extend_amount": op["extend_amount"],
                "flags": flags,
                "raw": op,
            })
        elif op["kind"] == "imm":
            role = op.get("role", 0)
            encoding = op.get("encoding", 0)
            kind = "target" if role in {6, 7} or encoding == 4 else "imm"
            if (
                arch == "arm64"
                and seen_a64_vector_list
                and asm_mnemonic.startswith(("ld", "st"))
                and role == 5
                and not saw_a64_vector_lane_imm
                and (op.get("info_flags", 0) & ASMKIT_OP_FLAGS["signed"]) == 0
            ):
                kind = "imm"
                saw_a64_vector_lane_imm = True
            elif arch in {"arm", "arm64"} and (asm_mnemonic.startswith(("ld", "st")) or asm_mnemonic == "prfm") and role == 5:
                kind = "mem-disp"
            value = op["abs_target"] if kind == "target" and op["abs_target"] != 0 else op["imm"]
            out.append({
                "kind": kind,
                "imm": value,
                "width": op["info_width"] or op["width"],
                "access": access,
                "raw": op,
            })
        elif op["kind"] == "pc-rel":
            value = op["abs_target"] if op["abs_target"] != 0 else op["imm"]
            out.append({
                "kind": "target",
                "imm": value,
                "width": op["info_width"] or op["width"],
                "access": access,
                "raw": op,
            })
        elif op["kind"] == "mem":
            mem_base = normalize_register_name(arch, op["mem_base_name"])
            mem_index = normalize_register_name(arch, op["mem_index_name"])
            mem_scale = op["mem_scale"] or 1
            if arch in {"arm", "arm64"} and mem_scale == 1 and op["shift_amount"] > 0:
                mem_scale = 1 << int(op["shift_amount"])
            if (
                arch == "arm64"
                and seen_a64_vector_list
                and asm_mnemonic.startswith(("ld", "st"))
                and mem_base
                and not mem_index
                and op["mem_disp"] == 0
            ):
                out.append({
                    "kind": "reg",
                    "reg": mem_base,
                    "width": op["info_width"] or op["width"],
                    "access": access,
                    "shift_kind": op["shift_kind"],
                    "shift_amount": op["shift_amount"],
                    "shift_reg": normalize_register_name(arch, op["shift_reg_name"]),
                    "extend_kind": op["extend_kind"],
                    "extend_amount": op["extend_amount"],
                    "flags": flags,
                    "raw": op,
                })
                continue
            out.append({
                "kind": "mem",
                "base": mem_base,
                "index": mem_index,
                "scale": mem_scale,
                "disp": op["mem_disp"],
                "segment": normalize_register_name(arch, op["mem_segment_name"]),
                "width": op["info_width"] or op["width"],
                "access": access,
                "raw": op,
            })
        else:
            out.append({"kind": op["kind"], "width": op["width"], "access": access, "raw": op})
    if arch == "arm64" and saw_a64_vector_postindex_xzr and not any(op["kind"] == "mem-disp" for op in out):
        vector_bytes = sum(((op.get("width", 0) or 0) * len(op.get("regs", ())) // 8) for op in out if op["kind"] == "reglist")
        if vector_bytes:
            out.append({"kind": "mem-disp", "imm": vector_bytes, "width": 32, "access": 0})
    if (
        arch in {"arm", "arm64"} and
        (asm_mnemonic.startswith(("ld", "st")) or asm_mnemonic == "prfm") and
        any(op["kind"] == "mem-disp" for op in out) and
        not any(op["kind"] == "mem" for op in out) and
        not any(op["kind"] == "mem-base" for op in out)
    ):
        disp_indexes = [i for i, op in enumerate(out) if op["kind"] == "mem-disp"]
        signed_disp = next(
            (
                i for i in disp_indexes
                if (out[i].get("raw", {}).get("info_flags", 0) & ASMKIT_OP_FLAGS["signed"]) != 0
            ),
            disp_indexes[0] if disp_indexes else len(out),
        )
        for index in range(signed_disp - 1, -1, -1):
            if out[index]["kind"] == "reg":
                out[index]["kind"] = "mem-base"
                break
    return out


def operand_to_atoms(arch: str, operand, source: str):
    kind = operand["kind"]
    atoms = []
    if kind == "reg":
        reg = operand.get("reg", "")
        atoms.append(("reg", reg))
        if source == "capstone" and arch == "arm64":
            lane = operand.get("vector_index", -1)
            if lane is not None and lane >= 0:
                atoms.append(("imm", int(lane)))
        if source == "asmkit" and arch == "arm64" and (operand.get("flags", 0) & ASMKIT_OP_FLAGS["register-pair"]) != 0:
            match = re.fullmatch(r"([wx])([0-9]+)", reg)
            if match is not None:
                atoms.append(("reg", f"{match.group(1)}{int(match.group(2)) + 1}"))
    elif kind == "mem-base":
        atoms.append(("mem-base", operand.get("reg", "")))
    elif kind == "reglist":
        for reg in operand.get("regs", ()):
            atoms.append(("reg", normalize_register_name(arch, reg)))
    elif kind == "imm":
        atoms.append(("imm", operand.get("imm", 0)))
    elif kind == "target":
        atoms.append(("target", operand.get("imm", 0)))
    elif kind == "mem-disp":
        if operand.get("imm", 0) != 0:
            atoms.append(("mem-disp", operand.get("imm", 0)))
    elif kind == "mem":
        if operand.get("base"):
            atoms.append(("mem-base", operand["base"]))
        has_index = bool(operand.get("index"))
        if has_index:
            atoms.append(("mem-index", operand["index"]))
        if has_index and operand.get("scale", 1) not in {0, 1}:
            atoms.append(("mem-scale", operand["scale"]))
        if operand.get("disp", 0) != 0:
            atoms.append(("mem-disp", operand["disp"]))
        if operand.get("segment"):
            atoms.append(("mem-segment", operand["segment"]))
    return atoms


def operand_atoms(arch: str, operands, source: str) -> Counter:
    atoms = []
    for operand in operands:
        atoms.extend(operand_to_atoms(arch, operand, source))
    return Counter(atoms)


def operand_access_atoms(arch: str, operands, access_bit: int) -> Counter:
    atoms = Counter()
    for operand in operands:
        if (operand.get("access", 0) & access_bit) == 0:
            continue
        kind = operand.get("kind")
        if kind == "reg":
            reg = operand.get("reg", "")
            if reg:
                atoms[("reg", reg)] += 1
        elif kind == "mem-base":
            reg = operand.get("reg", "")
            if reg:
                atoms[("mem-base", reg)] += 1
        elif kind == "reglist":
            for reg in operand.get("regs", ()):
                atoms[("reg", normalize_register_name(arch, reg))] += 1
        elif kind == "mem":
            width = operand.get("width", 0) or 0
            atoms[("mem", width)] += 1
    return normalize_atoms_for_match(arch, atoms)


def asmkit_register_effect_atoms(arch: str, asm_item) -> Counter:
    atoms = Counter()
    for effect in asm_item.get("effects_parsed", []):
        reg = normalize_register_name(arch, effect.get("reg_name", ""))
        if reg:
            atoms[("reg", reg)] += 1
    return atoms


def operand_value_equal(left, right) -> bool:
    if left["kind"] != right["kind"]:
        return False
    if left["kind"] == "reg":
        return left.get("reg", "") == right.get("reg", "")
    if left["kind"] == "target":
        return values_equivalent(left.get("imm", 0), right.get("imm", 0), left.get("width", 0), right.get("width", 0))
    if left["kind"] == "imm":
        return values_equivalent(left.get("imm", 0), right.get("imm", 0), left.get("width", 0), right.get("width", 0))
    if left["kind"] == "mem":
        return (
            left.get("base", "") == right.get("base", "") and
            left.get("index", "") == right.get("index", "") and
            left.get("scale", 1) == right.get("scale", 1) and
            values_equivalent(left.get("disp", 0), right.get("disp", 0), left.get("width", 0), right.get("width", 0)) and
            left.get("segment", "") == right.get("segment", "")
        )
    return True


def comparable_sequence(cap_ops, asm_ops) -> bool:
    if len(cap_ops) != len(asm_ops):
        return False
    for cap_op, asm_op in zip(cap_ops, asm_ops):
        if cap_op["kind"] != asm_op["kind"]:
            return False
        if not operand_value_equal(cap_op, asm_op):
            return False
    return True


def filtered_a64_ops_for_alias(cap_mnemonic: str, asm_mnemonic: str, cap_ops, asm_ops):
    if cap_mnemonic == "cmp" and asm_mnemonic == "subs" and len(asm_ops) >= 3:
        if asm_ops[0]["kind"] == "reg" and asm_ops[0].get("reg") in {"wzr", "xzr"}:
            compared = [asm_ops[1], dict(asm_ops[2])]
            if len(asm_ops) > 3 and compared[1]["kind"] == "imm" and asm_ops[3]["kind"] == "imm":
                compared[1]["imm"] = compared[1].get("imm", 0) << asm_ops[3].get("imm", 0)
            return cap_ops, compared, "cmp-subs-zero-dest"
    if cap_mnemonic == "cmn" and asm_mnemonic == "adds" and len(asm_ops) >= 3:
        if asm_ops[0]["kind"] == "reg" and asm_ops[0].get("reg") in {"wzr", "xzr"}:
            compared = [asm_ops[1], dict(asm_ops[2])]
            if len(asm_ops) > 3 and compared[1]["kind"] == "imm" and asm_ops[3]["kind"] == "imm":
                compared[1]["imm"] = compared[1].get("imm", 0) << asm_ops[3].get("imm", 0)
            return cap_ops, compared, "cmn-adds-zero-dest"
    if cap_mnemonic == "tst" and asm_mnemonic == "ands" and len(asm_ops) >= 3:
        if asm_ops[0]["kind"] == "reg" and asm_ops[0].get("reg") in {"wzr", "xzr"}:
            return cap_ops, asm_ops[1:3], "tst-ands-zero-dest"
    if cap_mnemonic == "mov" and asm_mnemonic == "orr" and len(cap_ops) == 2 and len(asm_ops) >= 3:
        if asm_ops[1]["kind"] == "reg" and asm_ops[1].get("reg") in {"wzr", "xzr"}:
            return cap_ops, [asm_ops[0], asm_ops[2]], "mov-orr-zero-source"
        if asm_ops[2]["kind"] == "reg" and asm_ops[2].get("reg") in {"wzr", "xzr"}:
            return cap_ops, [asm_ops[0], asm_ops[1]], "mov-orr-zero-source"
    if cap_mnemonic == "mov" and asm_mnemonic == "add" and len(cap_ops) == 2 and len(asm_ops) >= 3:
        if asm_ops[2]["kind"] == "imm" and values_equivalent(asm_ops[2].get("imm", 0), 0, asm_ops[2].get("width", 0), 0):
            return cap_ops, [asm_ops[0], asm_ops[1]], "mov-add-zero-imm"
    if cap_mnemonic == "mov" and asm_mnemonic == "movn" and len(cap_ops) >= 2 and len(asm_ops) >= 2:
        if cap_ops[1]["kind"] == "imm" and asm_ops[1]["kind"] == "imm":
            dest = asm_ops[0].get("reg", "") if asm_ops[0]["kind"] == "reg" else ""
            width = 32 if dest.startswith("w") else 64
            shift = asm_ops[2].get("imm", 0) if len(asm_ops) > 2 and asm_ops[2]["kind"] == "imm" else 0
            value = (~(asm_ops[1].get("imm", 0) << shift)) & ((1 << width) - 1)
            signed = sign_extend(value, width)
            if cap_ops[0]["kind"] == "reg" and operand_value_equal(cap_ops[0], asm_ops[0]) and cap_ops[1].get("imm", 0) in {value, signed}:
                return cap_ops[:1], asm_ops[:1], "mov-movn-materialized-imm"
    if cap_mnemonic == "mov" and asm_mnemonic == "movz" and len(cap_ops) >= 2 and len(asm_ops) >= 2:
        if cap_ops[1]["kind"] == "imm" and asm_ops[1]["kind"] == "imm":
            dest = asm_ops[0].get("reg", "") if asm_ops[0]["kind"] == "reg" else ""
            width = 32 if dest.startswith("w") else 64
            shift = asm_ops[2].get("imm", 0) if len(asm_ops) > 2 and asm_ops[2]["kind"] == "imm" else 0
            value = (asm_ops[1].get("imm", 0) << shift) & ((1 << width) - 1)
            if cap_ops[0]["kind"] == "reg" and operand_value_equal(cap_ops[0], asm_ops[0]) and values_equivalent(cap_ops[1].get("imm", 0), value, cap_ops[1].get("width", 0), width):
                return cap_ops[:1], asm_ops[:1], "mov-movz-materialized-imm"
    if cap_mnemonic == "mov" and asm_mnemonic in {"movz", "movn", "movk"} and len(cap_ops) >= 2 and len(asm_ops) >= 2:
        return cap_ops[:2], asm_ops[:2], f"mov-{asm_mnemonic}-dest-imm"
    if cap_mnemonic == "mov" and asm_mnemonic == "dup" and len(cap_ops) == 2 and len(asm_ops) >= 2:
        if all(a64_operand_value_equal_allow_vector_family(cap_op, asm_op) for cap_op, asm_op in zip(cap_ops, asm_ops[:2])):
            if len(asm_ops) == 2 or (len(asm_ops) == 3 and asm_ops[2]["kind"] == "imm" and asm_ops[2].get("imm", -1) == 0):
                return cap_ops, asm_ops[:2], "mov-dup-zero-lane"
    if cap_mnemonic == "mov" and asm_mnemonic == "sel" and len(cap_ops) == 3 and len(asm_ops) >= 4:
        if (
            all(a64_operand_value_equal_allow_vector_family(cap_op, asm_op) for cap_op, asm_op in zip(cap_ops, asm_ops[:3]))
            and a64_operand_value_equal_allow_vector_family(cap_ops[0], asm_ops[3])
        ):
            return cap_ops, asm_ops[:3], "mov-sel-tied-dest"
    if cap_mnemonic == "not" and asm_mnemonic == "eor" and len(cap_ops) == 3 and len(asm_ops) >= 4:
        if (
            all(a64_operand_value_equal_allow_vector_family(cap_op, asm_op) for cap_op, asm_op in zip(cap_ops, asm_ops[:3]))
            and a64_operand_value_equal_allow_vector_family(asm_ops[1], asm_ops[3])
        ):
            return cap_ops, asm_ops[:3], "not-eor-merge-predicate"
    if cap_mnemonic in {"ngc", "ngcs"} and asm_mnemonic in {"sbc", "sbcs"} and len(cap_ops) == 2 and len(asm_ops) >= 3:
        zero_reg = "wzr" if str(asm_ops[0].get("reg", "")).startswith("w") else "xzr"
        if (
            cap_ops[0]["kind"] == "reg"
            and cap_ops[1]["kind"] == "reg"
            and asm_ops[0]["kind"] == "reg"
            and asm_ops[1]["kind"] == "reg"
            and asm_ops[2]["kind"] == "reg"
            and operand_value_equal(cap_ops[0], asm_ops[0])
            and asm_ops[1].get("reg") == zero_reg
            and operand_value_equal(cap_ops[1], asm_ops[2])
        ):
            return cap_ops, [asm_ops[0], asm_ops[2]], f"{cap_mnemonic}-{asm_mnemonic}-zero-source"
    if cap_mnemonic in {"uxtb", "uxth", "sxtb", "sxth", "sxtw"} and asm_mnemonic in {"ubfm", "sbfm"} and len(cap_ops) == 2 and len(asm_ops) >= 4:
        expected = {"uxtb": 7, "sxtb": 7, "uxth": 15, "sxth": 15, "sxtw": 31}[cap_mnemonic]
        if asm_ops[2]["kind"] == "imm" and asm_ops[3]["kind"] == "imm" and asm_ops[2].get("imm") == 0 and asm_ops[3].get("imm") == expected:
            return cap_ops, asm_ops[:2], f"{cap_mnemonic}-{asm_mnemonic}-range"
    if cap_mnemonic in {"ubfx", "sbfx"} and asm_mnemonic in {"ubfm", "sbfm"} and len(cap_ops) == 4 and len(asm_ops) >= 4:
        if cap_ops[2]["kind"] == "imm" and cap_ops[3]["kind"] == "imm" and asm_ops[2]["kind"] == "imm" and asm_ops[3]["kind"] == "imm":
            lsb = cap_ops[2].get("imm", 0)
            width = cap_ops[3].get("imm", 0)
            if asm_ops[2].get("imm") == lsb and asm_ops[3].get("imm") == lsb + width - 1:
                return cap_ops[:2], asm_ops[:2], f"{cap_mnemonic}-{asm_mnemonic}-range"
    if cap_mnemonic.startswith("bc.") and asm_mnemonic == "bc" and len(asm_ops) >= 2:
        cond = A64_CONDITIONS.get(cap_mnemonic.split(".", 1)[1])
        if cond is not None and asm_ops[0]["kind"] == "imm" and values_equivalent(asm_ops[0].get("imm", -1), cond, asm_ops[0].get("width", 0), 0):
            return cap_ops, [asm_ops[1]], "bc-condition-operand"
    return None


def filtered_arm_ops_for_alias(cap_mnemonic: str, asm_mnemonic: str, cap_ops, asm_ops):
    cap_base = strip_arm_it_condition_suffix(cap_mnemonic) or cap_mnemonic
    cap_no_flags = cap_base[:-1] if cap_base.endswith("s") and cap_base[:-1] in ARM_SETFLAGS_BASES else cap_base
    def collected_regs(ops):
        regs = []
        for op in ops:
            if op["kind"] == "reglist":
                regs.extend(op.get("regs", ()))
            elif op["kind"] == "reg":
                regs.append(op.get("reg", ""))
        return tuple(reg for reg in regs if reg)
    if cap_no_flags in {"lsr", "asr"} and asm_mnemonic == cap_no_flags and len(cap_ops) >= 3 and len(asm_ops) >= 3:
        if cap_ops[2]["kind"] == "imm" and asm_ops[2]["kind"] == "imm" and cap_ops[2].get("imm") == 32 and asm_ops[2].get("imm") == 0:
            return cap_ops[:2], asm_ops[:2], "arm-shift-imm-zero-means-32"
    if cap_base == asm_mnemonic:
        return cap_ops, asm_ops[:len(cap_ops)], "arm-condition-suffix"
    if cap_no_flags == asm_mnemonic:
        return cap_ops, asm_ops[:len(cap_ops)], "arm-condition-setflags-suffix"
    if cap_no_flags in {"lsl", "lsr", "asr", "ror"} and asm_mnemonic == "mov" and len(cap_ops) >= 2 and len(asm_ops) >= 2:
        cap_shift_amount = int(cap_ops[1].get("shift_amount", 0))
        if len(cap_ops) >= 3 and cap_ops[2].get("kind") == "imm":
            cap_shift_amount = int(cap_ops[2].get("imm", 0))
        if (
            cap_ops[0].get("kind") == "reg"
            and cap_ops[1].get("kind") == "reg"
            and asm_ops[0].get("kind") == "reg"
            and asm_ops[1].get("kind") == "reg"
            and operand_value_equal(cap_ops[0], asm_ops[0])
            and operand_value_equal(cap_ops[1], asm_ops[1])
            and int(asm_ops[1].get("shift_amount", 0)) == cap_shift_amount
        ):
            return cap_ops[:2], asm_ops[:2], f"{cap_no_flags}-mov-shifted-register"
    if cap_base.endswith("s") and cap_base[:-1] == asm_mnemonic:
        return cap_ops, asm_ops[:len(cap_ops)], "arm-setflags-suffix"
    if cap_base == asm_mnemonic + "s":
        return cap_ops, asm_ops[:len(cap_ops)], "arm-setflags-suffix"
    if cap_base == "pop" and asm_mnemonic == "ldr" and len(asm_ops) >= 1:
        cap_regs = collected_regs(cap_ops)
        if len(cap_regs) == 1 and asm_ops[0].get("kind") == "reg" and asm_ops[0].get("reg") == cap_regs[0]:
            if any(op.get("kind") in {"mem", "mem-base"} and op.get("base", op.get("reg", "")) == "sp" for op in asm_ops[1:]):
                return [{"kind": "reglist", "regs": cap_regs}], [{"kind": "reglist", "regs": cap_regs}], "pop-ldr-postindex"
    if cap_base in {"push", "pop"} and asm_mnemonic in {"stmdb", "ldm", "ldmia"}:
        cap_regs = tuple(op["reg"] for op in cap_ops if op["kind"] == "reg")
        asm_regs = ()
        for op in asm_ops:
            if op["kind"] == "reglist":
                asm_regs = op.get("regs", ())
                break
        return [{"kind": "reglist", "regs": cap_regs}], [{"kind": "reglist", "regs": asm_regs}], f"{cap_base}-{asm_mnemonic}-reglist"
    if cap_base == "vpush" and asm_mnemonic == "vstmdb":
        return [{"kind": "reglist", "regs": collected_regs(cap_ops)}], [{"kind": "reglist", "regs": collected_regs(asm_ops[1:])}], "vpush-vstmdb-reglist"
    if cap_base == "vpop" and asm_mnemonic in {"vldm", "vldmia"}:
        return [{"kind": "reglist", "regs": collected_regs(cap_ops)}], [{"kind": "reglist", "regs": collected_regs(asm_ops[1:])}], f"vpop-{asm_mnemonic}-reglist"
    return None


def arm_barrier_operands_match(cap_insn, asm_ops) -> bool:
    text = cap_insn.op_str.strip().lower() or "sy"
    option = text.split(",", 1)[0].strip()
    expected = BARRIER_OPTION_IMMS.get(option)
    return (
        expected is not None
        and len(asm_ops) == 1
        and asm_ops[0]["kind"] == "imm"
        and values_equivalent(asm_ops[0].get("imm", -1), expected, asm_ops[0].get("width", 0), 0)
    )


def arm_it_operands_match(cap_insn, asm_ops) -> bool:
    condition_text = cap_insn.op_str.strip().split(None, 1)[0].lower()
    condition = A64_CONDITIONS.get(condition_text)
    return (
        condition is not None
        and len(asm_ops) >= 2
        and asm_ops[0]["kind"] == "imm"
        and asm_ops[1]["kind"] == "imm"
        and values_equivalent(asm_ops[0].get("imm", -1), condition, asm_ops[0].get("width", 0), 0)
        and asm_ops[1].get("imm", 0) != 0
    )


def arm_cps_operands_match(cap_mnemonic: str, cap_insn, asm_ops) -> bool:
    if len(asm_ops) < 2 or asm_ops[0]["kind"] != "imm" or asm_ops[1]["kind"] != "imm":
        return False
    expected_enable = 0 if cap_mnemonic == "cpsie" else 1
    masks = cap_insn.op_str.strip().lower()
    expected_mask = 0
    if "f" in masks:
        expected_mask |= 1
    if "i" in masks:
        expected_mask |= 2
    if "a" in masks:
        expected_mask |= 4
    return (
        values_equivalent(asm_ops[0].get("imm", -1), expected_enable, asm_ops[0].get("width", 0), 0)
        and (expected_mask == 0 or (asm_ops[1].get("imm", 0) & expected_mask) == expected_mask)
    )


def arm_msr_operands_match(cap_ops, asm_ops) -> bool:
    if len(cap_ops) != 2 or len(asm_ops) != 2:
        return False
    if cap_ops[0].get("kind") != "imm" or asm_ops[0].get("kind") != "imm":
        return False
    if (int(asm_ops[0].get("imm", -1)) & 0x0f) != int(cap_ops[0].get("imm", -1)):
        return False
    return cap_ops[1].get("kind") == "reg" and asm_ops[1].get("kind") == "reg" and cap_ops[1].get("reg") == asm_ops[1].get("reg")


def capstone_lsl_shift_amount(op_str: str) -> int:
    match = re.search(r"\blsl\s+#(0x[0-9a-fA-F]+|\d+)\b", op_str)
    if match is None:
        return 0
    return int(match.group(1), 0)


def capstone_memory_scale(arch: str, insn, op, default_scale: int) -> int:
    scale = default_scale or 1
    if scale not in {0, 1} or arch not in {"arm", "arm64"}:
        return scale
    shift = getattr(op, "shift", None)
    shift_kind = getattr(shift, "type", 0) if shift is not None else 0
    shift_amount = getattr(shift, "value", 0) if shift is not None else 0
    if shift_kind != 0 and shift_amount > 0:
        return 1 << int(shift_amount)
    if arch == "arm" and getattr(getattr(op, "mem", None), "index", 0):
        text_shift = capstone_lsl_shift_amount(insn.op_str)
        if text_shift > 0:
            return 1 << text_shift
    return scale


def a64_vector_reg_equivalent(left: str, right: str) -> bool:
    if left == right:
        return True
    left_match = re.fullmatch(r"[bhsdqvz]([0-9]+)", left or "")
    right_match = re.fullmatch(r"[bhsdqvz]([0-9]+)", right or "")
    return left_match is not None and right_match is not None and left_match.group(1) == right_match.group(1)


def a64_operand_value_equal_allow_vector_family(left, right) -> bool:
    if left["kind"] != right["kind"]:
        return False
    if left["kind"] == "reg":
        left_reg = left.get("reg", "")
        right_reg = right.get("reg", "")
        return left_reg == right_reg or a64_vector_reg_equivalent(left_reg, right_reg) or a64_reg_family_equivalent(left_reg, right_reg)
    return operand_value_equal(left, right)


def a64_vector_lane_imm_equivalent(cap_ops, asm_ops) -> bool:
    asm_index = 0
    used_asm = [False] * len(asm_ops)
    for cap_op in cap_ops:
        if asm_index >= len(asm_ops):
            return False
        asm_op = asm_ops[asm_index]
        if not a64_operand_value_equal_allow_vector_family(cap_op, asm_op):
            return False
        used_asm[asm_index] = True
        asm_index += 1
        lane = cap_op.get("vector_index", -1)
        if cap_op.get("kind") == "reg" and lane is not None and lane >= 0:
            if asm_index >= len(asm_ops) or asm_ops[asm_index].get("kind") != "imm":
                return False
            if not values_equivalent(asm_ops[asm_index].get("imm", -1), lane, asm_ops[asm_index].get("width", 0), 0):
                return False
            used_asm[asm_index] = True
            asm_index += 1
    return asm_index == len(asm_ops) and all(used_asm)


def default_zero_imm_equivalent(cap_mnemonic: str, asm_mnemonic: str, cap_ops, asm_ops) -> bool:
    if cap_mnemonic != asm_mnemonic:
        return False
    if cap_mnemonic in A64_FIXED_ZERO_IMM_MNEMONICS and len(cap_ops) == len(asm_ops) + 1:
        return cap_ops[-1].get("kind") == "imm" and cap_ops[-1].get("imm", -1) == 0 and comparable_sequence(cap_ops[:-1], asm_ops)
    if cap_mnemonic in {"movi", "mvni"} and len(asm_ops) == len(cap_ops) + 1:
        return (
            asm_ops[-1].get("kind") == "imm"
            and asm_ops[-1].get("imm", -1) == 0
            and all(a64_operand_value_equal_allow_vector_family(cap_op, asm_op) for cap_op, asm_op in zip(cap_ops, asm_ops[:-1]))
        )
    if cap_mnemonic in ARM_DEFAULT_ZERO_IMM_MNEMONICS and len(asm_ops) == len(cap_ops) + 1:
        return asm_ops[-1].get("kind") == "imm" and asm_ops[-1].get("imm", -1) == 0 and comparable_sequence(cap_ops, asm_ops[:-1])
    return False


def a64_fpimm_encoding_equivalent(cap_mnemonic: str, cap_ops, asm_ops) -> bool:
    return (
        cap_mnemonic == "fmov"
        and len(cap_ops) == 2
        and len(asm_ops) == 2
        and cap_ops[0].get("kind") == "reg"
        and asm_ops[0].get("kind") == "reg"
        and a64_operand_value_equal_allow_vector_family(cap_ops[0], asm_ops[0])
        and str(cap_ops[1].get("kind", "")).startswith("capstone-op-")
        and asm_ops[1].get("kind") == "imm"
    )


def arm_vfp_fpimm_encoding_equivalent(cap_mnemonic: str, cap_ops, asm_ops) -> bool:
    return (
        cap_mnemonic.startswith("vmov")
        and len(cap_ops) == 2
        and len(asm_ops) == 2
        and cap_ops[0].get("kind") == "reg"
        and asm_ops[0].get("kind") == "reg"
        and operand_value_equal(cap_ops[0], asm_ops[0])
        and str(cap_ops[1].get("kind", "")).startswith("capstone-op-")
        and asm_ops[1].get("kind") == "imm"
    )


def arm_vcvt_fixed_point_immediate_equivalent(cap_mnemonic: str, cap_ops, asm_ops) -> bool:
    if not cap_mnemonic.startswith("vcvt.") or len(cap_ops) != 3 or len(asm_ops) != 2:
        return False
    if cap_ops[0].get("kind") != "reg" or cap_ops[1].get("kind") != "reg" or asm_ops[0].get("kind") != "reg":
        return False
    if not operand_value_equal(cap_ops[0], asm_ops[0]) or not operand_value_equal(cap_ops[1], asm_ops[0]):
        return False
    if cap_ops[2].get("kind") != "imm" or asm_ops[1].get("kind") != "imm":
        return False
    width_match = re.search(r"\.(?:s|u)(16|32)\b", cap_mnemonic)
    lane_width = int(width_match.group(1)) if width_match is not None else 32
    return int(cap_ops[2].get("imm", -1)) + int(asm_ops[1].get("imm", -1)) == lane_width


def arm_postindex_register_detail_equivalent(cap_insn, cap_ops, asm_ops) -> bool:
    mem_positions = [
        index for index, op in enumerate(cap_ops)
        if op.get("kind") == "mem"
        and op.get("base")
        and not op.get("index")
        and int(op.get("disp", 0)) == 0
    ]
    if len(mem_positions) != 1:
        return False
    mem_index = mem_positions[0]
    if mem_index + 1 >= len(cap_ops):
        return False
    cap_offset = cap_ops[mem_index + 1]
    if cap_offset.get("kind") != "reg":
        return False
    if mem_index + 2 != len(cap_ops):
        return False
    if len(asm_ops) < mem_index + 3:
        return False
    if not comparable_sequence(cap_ops[:mem_index], asm_ops[:mem_index]):
        return False

    cap_base = cap_ops[mem_index].get("base", "")
    cap_index = cap_offset.get("reg", "")
    asm_tail = asm_ops[mem_index:]
    base_mem = None
    index_mem = None
    writeback_base = None
    for index, op in enumerate(asm_tail):
        if (
            base_mem is None
            and op.get("kind") == "mem"
            and op.get("base") == cap_base
            and not op.get("index")
            and int(op.get("disp", 0)) == 0
        ):
            base_mem = index
            continue
        if (
            index_mem is None
            and op.get("kind") == "mem"
            and not op.get("base")
            and op.get("index") == cap_index
            and int(op.get("disp", 0)) == 0
        ):
            index_mem = index
            continue
        if (
            writeback_base is None
            and op.get("kind") in {"reg", "mem-base"}
            and op.get("reg") == cap_base
            and (int(op.get("access", 0)) & 2) != 0
        ):
            writeback_base = index
    if base_mem is None or index_mem is None or writeback_base is None:
        return False
    if {base_mem, index_mem, writeback_base} != set(range(len(asm_tail))):
        return False

    cap_negative = re.search(r"\],\s*-", cap_insn.op_str.lower()) is not None
    asm_negative = (
        int(asm_tail[index_mem].get("raw", {}).get("flags", 0))
        & ASMKIT_OP_FLAGS["arm-negative-offset"]
    ) != 0
    return cap_negative == asm_negative


def arm_postindex_immediate_detail_equivalent(cap_insn, cap_ops, asm_ops) -> bool:
    match = re.search(r"\],\s*#(-?0x[0-9a-fA-F]+|-?\d+)\b", cap_insn.op_str.lower())
    if match is None:
        return False
    post_imm = int(match.group(1), 0)
    cap_core_ops = list(cap_ops)
    if cap_core_ops and cap_core_ops[-1].get("kind") == "mem-disp":
        cap_core_ops = cap_core_ops[:-1]
    mem_positions = [
        index for index, op in enumerate(cap_core_ops)
        if op.get("kind") == "mem"
        and op.get("base")
        and not op.get("index")
        and int(op.get("disp", 0)) == 0
    ]
    if len(mem_positions) != 1:
        return False
    mem_index = mem_positions[0]
    if mem_index + 1 != len(cap_core_ops):
        return False
    if len(asm_ops) < mem_index + 3:
        return False
    if not comparable_sequence(cap_core_ops[:mem_index], asm_ops[:mem_index]):
        return False
    cap_base = cap_core_ops[mem_index].get("base", "")
    asm_tail = asm_ops[mem_index:]
    base_mem = None
    offset_imm = None
    writeback_base = None
    for index, op in enumerate(asm_tail):
        if (
            base_mem is None
            and op.get("kind") == "mem"
            and op.get("base") == cap_base
            and not op.get("index")
            and int(op.get("disp", 0)) == 0
        ):
            base_mem = index
            continue
        if offset_imm is None and op.get("kind") in {"imm", "mem-disp"} and int(op.get("imm", 0)) == post_imm:
            offset_imm = index
            continue
        if (
            writeback_base is None
            and op.get("kind") in {"reg", "mem-base"}
            and op.get("reg") == cap_base
            and (int(op.get("access", 0)) & 2) != 0
        ):
            writeback_base = index
    if base_mem is None or offset_imm is None or writeback_base is None:
        return False
    return {base_mem, offset_imm, writeback_base} == set(range(len(asm_tail)))


def arm_vector_alignment_detail_equivalent(cap_insn, cap_mnemonic: str, cap_ops, asm_ops) -> bool:
    if not cap_mnemonic.startswith(("vld", "vst")):
        return False
    align_match = re.search(r"\[[^\]]+:(0x[0-9a-fA-F]+|\d+)\]", cap_insn.op_str)
    if align_match is None:
        return False
    alignment = int(align_match.group(1), 0)
    adjusted_cap_ops = []
    adjusted = False
    for op in cap_ops:
        item = dict(op)
        if item.get("kind") == "mem" and int(item.get("disp", 0)) == alignment:
            item["disp"] = 0
            adjusted = True
        adjusted_cap_ops.append(item)
    if not adjusted:
        return False
    return atom_counters_match_allow_extra_detail(
        "arm",
        operand_atoms("arm", adjusted_cap_ops, "capstone"),
        operand_atoms("arm", asm_ops, "asmkit"),
    )


def arm_coprocessor_apsr_alias_equivalent(cap_mnemonic: str, cap_ops, asm_ops) -> bool:
    if cap_mnemonic not in {"mrc", "mrc2"} or len(cap_ops) != len(asm_ops):
        return False
    adjusted = []
    for op in cap_ops:
        item = dict(op)
        if item.get("kind") == "reg" and item.get("reg") == "apsr_nzcv":
            item["reg"] = "pc"
        adjusted.append(item)
    return comparable_sequence(adjusted, asm_ops)


def arm_extend_rotate_encoding_equivalent(cap_insn, cap_mnemonic: str, cap_ops, asm_ops) -> bool:
    if cap_mnemonic not in {"sxtb", "sxtb16", "sxth", "uxtb", "uxtb16", "uxth"}:
        return False
    if len(cap_ops) != 2 or len(asm_ops) != 3:
        return False
    if not comparable_sequence(cap_ops, asm_ops[:2]):
        return False
    if asm_ops[2].get("kind") != "imm":
        return False
    match = re.search(r"\bror\s+#(0x[0-9a-fA-F]+|\d+)\b", cap_insn.op_str.lower())
    if match is None:
        return False
    return int(asm_ops[2].get("imm", -1)) * 8 == int(match.group(1), 0)


def arm_vector_lane_immediate_equivalent(cap_insn, cap_ops, asm_ops) -> bool:
    lane_values = [int(text, 0) for text in re.findall(r"\b[qd][0-9]+\[(\d+)\]", cap_insn.op_str.lower())]
    if not lane_values:
        return False
    cap_atoms = normalize_atoms_for_match("arm", operand_atoms("arm", cap_ops, "capstone"))
    asm_atoms = normalize_atoms_for_match("arm", operand_atoms("arm", asm_ops, "asmkit"))
    for lane in lane_values:
        cap_atoms[("imm", lane)] += 1
    return atom_counters_match_allow_extra_detail("arm", cap_atoms, asm_atoms)


def arm_vector_memory_detail_equivalent(cap_insn, cap_mnemonic: str, cap_ops, asm_ops) -> bool:
    if not cap_mnemonic.startswith(("vld", "vst")) or "{" not in cap_insn.op_str:
        return False
    align_match = re.search(r"\[[^\]]+:(0x[0-9a-fA-F]+|\d+)\]", cap_insn.op_str)
    alignment = int(align_match.group(1), 0) if align_match is not None else None
    cap_adjusted = []
    for op in cap_ops:
        item = dict(op)
        if alignment is not None and item.get("kind") == "mem" and int(item.get("disp", 0)) == alignment:
            item["disp"] = 0
        cap_adjusted.append(item)
    asm_adjusted = []
    for op in asm_ops:
        item = dict(op)
        raw = item.get("raw", {})
        if (
            alignment is not None
            and item.get("kind") == "mem"
            and item.get("base")
            and not item.get("index")
            and int(item.get("disp", 0)) in {int(raw.get("imm", 0)), alignment}
        ):
            item["disp"] = 0
        asm_adjusted.append(item)

    cap_atoms = normalize_atoms_for_match("arm", operand_atoms("arm", cap_adjusted, "capstone"))
    asm_atoms = normalize_atoms_for_match("arm", operand_atoms("arm", asm_adjusted, "asmkit"))
    for lane_text in re.findall(r"\b[qd][0-9]+\[(\d+)\]", cap_insn.op_str.lower()):
        lane_atom = ("imm", int(lane_text, 0))
        if asm_atoms.get(lane_atom, 0) > 0:
            asm_atoms[lane_atom] -= 1
    for atom, count in cap_atoms.items():
        remaining = count
        direct = min(remaining, asm_atoms.get(atom, 0))
        if direct:
            asm_atoms[atom] -= direct
            remaining -= direct
        if remaining and atom[0] == "reg":
            mem_atom = ("mem-index", atom[1])
            via_mem_index = min(remaining, asm_atoms.get(mem_atom, 0))
            if via_mem_index:
                asm_atoms[mem_atom] -= via_mem_index
                remaining -= via_mem_index
        if remaining:
            return False
    for atom, count in asm_atoms.items():
        if count <= 0:
            continue
        if atom == ("mem-index", "sp") and "]!" in cap_insn.op_str:
            continue
        if atom[0] == "mem-base" and cap_atoms.get(("mem-base", atom[1]), 0):
            continue
        if atom[0] == "reg" and cap_atoms.get(("mem-base", atom[1]), 0):
            continue
        return False
    return True


def a64_vector_imm_encoding_equivalent(cap_mnemonic: str, cap_ops, asm_ops) -> bool:
    return (
        cap_mnemonic in {"bic", "orr", "movi", "mvni"}
        and len(cap_ops) == 2
        and len(asm_ops) == 2
        and cap_ops[0].get("kind") == "reg"
        and asm_ops[0].get("kind") == "reg"
        and a64_operand_value_equal_allow_vector_family(cap_ops[0], asm_ops[0])
        and cap_ops[1].get("kind") == "imm"
        and asm_ops[1].get("kind") == "imm"
    )


def a64_vector_extra_immediate_equivalent(cap_mnemonic: str, cap_ops, asm_ops) -> bool:
    if cap_mnemonic not in {"bic", "orr", "movi", "mvni"} or len(asm_ops) <= len(cap_ops):
        return False
    if len(asm_ops) - len(cap_ops) > 2:
        return False
    if not all(op.get("kind") == "imm" for op in asm_ops[len(cap_ops):]):
        return False
    return all(
        a64_operand_value_equal_allow_vector_family(cap_op, asm_op)
        for cap_op, asm_op in zip(cap_ops, asm_ops[:len(cap_ops)])
    )


def a64_postindex_immediate(op_str: str) -> int | None:
    match = re.search(r"\],\s*#(0x[0-9a-fA-F]+|\d+)\b", op_str)
    if match is None:
        return None
    return int(match.group(1), 0)


def a64_vector_memory_detail_equivalent(cap_insn, cap_ops, asm_ops) -> bool:
    mnemonic = normalize_a64_mnemonic(cap_insn.mnemonic)
    op_str = cap_insn.op_str.strip().lower()
    if not mnemonic.startswith(("ld", "st")) or "{" not in op_str or "[" not in op_str:
        return False
    cap_atoms = normalize_atoms_for_match("arm64", operand_atoms("arm64", cap_ops, "capstone"))
    asm_atoms = normalize_atoms_for_match("arm64", operand_atoms("arm64", asm_ops, "asmkit"))
    post_imm = a64_postindex_immediate(op_str)
    for atom, count in cap_atoms.items():
        remaining = count
        direct = min(remaining, asm_atoms.get(atom, 0))
        if direct:
            asm_atoms[atom] -= direct
            remaining -= direct
        if remaining and atom[0] == "mem-base":
            reg_atom = ("reg", atom[1])
            via_reg = min(remaining, asm_atoms.get(reg_atom, 0))
            if via_reg:
                asm_atoms[reg_atom] -= via_reg
                remaining -= via_reg
        if remaining and atom[0] == "reg":
            mem_atom = ("mem-base", atom[1])
            via_mem = min(remaining, asm_atoms.get(mem_atom, 0))
            if via_mem:
                asm_atoms[mem_atom] -= via_mem
                remaining -= via_mem
        if remaining and atom[0] == "mem-index":
            reg_atom = ("reg", atom[1])
            via_reg = min(remaining, asm_atoms.get(reg_atom, 0))
            if via_reg:
                asm_atoms[reg_atom] -= via_reg
                remaining -= via_reg
            mem_base_atom = ("mem-base", atom[1])
            via_mem_base = min(remaining, asm_atoms.get(mem_base_atom, 0))
            if via_mem_base:
                asm_atoms[mem_base_atom] -= via_mem_base
                remaining -= via_mem_base
        if remaining and atom[0] == "mem-scale":
            remaining = 0
        if remaining:
            return False
    for atom, count in asm_atoms.items():
        if count <= 0:
            continue
        if atom[0] == "mem-disp" and post_imm is not None:
            continue
        if atom == ("reg", "xzr") and post_imm is not None:
            continue
        return False
    return True


def a64_sve_default_immediate_operands_equivalent(cap_insn, cap_mnemonic: str, asm_mnemonic: str, cap_ops, asm_ops) -> bool:
    if cap_mnemonic != asm_mnemonic or len(asm_ops) <= len(cap_ops):
        return False
    op_str = cap_insn.op_str.strip().lower()
    if (
        not cap_mnemonic.startswith(("ptrue", "cnt", "inc", "dec"))
        and " all" not in f" {op_str}"
        and "vl" not in op_str
        and "mul #" not in op_str
    ):
        return False
    cap_index = 0
    asm_index = 0
    skipped = []
    while cap_index < len(cap_ops) and asm_index < len(asm_ops):
        cap_op = cap_ops[cap_index]
        asm_op = asm_ops[asm_index]
        if a64_operand_value_equal_allow_vector_family(cap_op, asm_op):
            cap_index += 1
            asm_index += 1
            continue
        if asm_op.get("kind") != "imm":
            return False
        skipped.append(asm_op)
        asm_index += 1
    if cap_index != len(cap_ops):
        return False
    skipped.extend(asm_ops[asm_index:])
    return bool(skipped) and all(op.get("kind") == "imm" for op in skipped)


def a64_prfm_register_offset_detail_equivalent(cap_mnemonic: str, asm_mnemonic: str, cap_ops, asm_ops) -> bool:
    if cap_mnemonic != "prfm" or asm_mnemonic != "prfm" or len(cap_ops) != 1:
        return False
    cap_mem = cap_ops[0]
    if cap_mem.get("kind") != "mem" or not cap_mem.get("base") or not cap_mem.get("index"):
        return False
    asm_regs = [op.get("reg", "") for op in asm_ops if op.get("kind") == "reg"]
    if len(asm_regs) < 2:
        return False
    if asm_regs[0] != cap_mem.get("base") or asm_regs[1] != cap_mem.get("index"):
        return False
    return any(op.get("kind") == "opaque" for op in asm_ops)


def a64_opaque_immediate_encoding_equivalent(cap_mnemonic: str, cap_ops, asm_ops) -> bool:
    if cap_mnemonic not in {"fcadd"}:
        return False
    if not cap_ops or not asm_ops:
        return False
    if cap_ops[-1].get("kind") != "imm" or asm_ops[-1].get("kind") != "opaque":
        return False
    cap_prefix = cap_ops[:-1]
    asm_prefix = asm_ops[:-1]
    if len(cap_prefix) == len(asm_prefix) and comparable_sequence(cap_prefix, asm_prefix):
        return True
    return a64_tied_readwrite_operand_equivalent(cap_prefix, asm_prefix)


def a64_sve_incdec_default_operands_equivalent(cap_mnemonic: str, asm_mnemonic: str, cap_ops, asm_ops) -> bool:
    if cap_mnemonic != asm_mnemonic or len(asm_ops) != len(cap_ops) + 2:
        return False
    if not all(a64_operand_value_equal_allow_vector_family(cap_op, asm_op) for cap_op, asm_op in zip(cap_ops, asm_ops[:len(cap_ops)])):
        return False
    return (
        asm_ops[-2].get("kind") == "imm"
        and asm_ops[-1].get("kind") == "imm"
        and asm_ops[-2].get("imm", -1) == 31
        and asm_ops[-1].get("imm", -1) == 1
    )


def tied_readwrite_operand_equivalent(arch: str, cap_ops, asm_ops) -> bool:
    cap_atoms = normalize_atoms_for_match(arch, operand_atoms(arch, cap_ops, "capstone"))
    asm_atoms = normalize_atoms_for_match(arch, operand_atoms(arch, asm_ops, "asmkit"))
    missing = cap_atoms - asm_atoms
    if not missing:
        return False
    extra = asm_atoms - cap_atoms
    if extra:
        return False
    readwrite_atoms = Counter()
    for op in asm_ops:
        if (op.get("access", 0) & 3) != 3:
            continue
        for atom in operand_to_atoms(arch, op, "asmkit"):
            readwrite_atoms[normalized_atom_for_match(arch, atom)] += 1
    return all(atom[0] == "reg" and readwrite_atoms.get(atom, 0) >= count for atom, count in missing.items())


def a64_tied_readwrite_operand_equivalent(cap_ops, asm_ops) -> bool:
    return tied_readwrite_operand_equivalent("arm64", cap_ops, asm_ops)


def a64_sme_za_capstone_partial_equivalent(cap_insn, cap_ops, asm_ops) -> bool:
    if normalize_a64_mnemonic(cap_insn.mnemonic) not in {"ldr", "str"}:
        return False
    if not cap_insn.op_str.strip().lower().startswith("za["):
        return False
    if not any(str(op.get("kind", "")).startswith("capstone-op-") for op in cap_ops):
        return False
    if not any(op.get("kind") == "reg" and op.get("reg") == "za" for op in asm_ops):
        return False
    if not any(op.get("kind") == "reg" and re.fullmatch(r"w1[2-5]", op.get("reg", "")) for op in asm_ops):
        return False
    cap_atoms = operand_atoms("arm64", cap_ops, "capstone")
    asm_atoms = operand_atoms("arm64", asm_ops, "asmkit")
    if cap_atoms - asm_atoms:
        return False
    extra = normalize_atoms_for_match("arm64", asm_atoms) - normalize_atoms_for_match("arm64", cap_atoms)
    for atom, count in extra.items():
        if count <= 0:
            continue
        if atom == ("reg", "za"):
            continue
        if atom[0] == "reg" and re.fullmatch(r"w1[2-5]", str(atom[1])):
            continue
        return False
    return True


def implicit_fixed_operand_equivalent(arch: str, mnemonic: str, cap_ops, asm_ops) -> bool:
    if arch == "arm64" and mnemonic == "clrex" and not cap_ops and len(asm_ops) == 1:
        return asm_ops[0].get("kind") == "imm" and values_equivalent(asm_ops[0].get("imm", -1), 15, asm_ops[0].get("width", 0), 0)
    return False


def pc_relative_target_matches(arch: str, mode: str, cap_insn, cap_ops, asm_ops) -> bool:
    cap_mem = None
    asm_target = None
    for op in cap_ops:
        if op.get("kind") == "mem" and op.get("base") == "pc":
            cap_mem = op
            break
    for op in asm_ops:
        if op.get("kind") == "target":
            asm_target = op
            break
    if cap_mem is None or asm_target is None:
        return False
    disp = int(cap_mem.get("disp", 0))
    address = int(cap_insn.address)
    if arch == "arm64":
        candidates = (address + disp,)
    elif mode == "thumb":
        candidates = (((address + 4) & ~3) + disp,)
    elif mode == "a32":
        candidates = (address + 8 + disp,)
    else:
        candidates = (address + 8 + disp, ((address + 4) & ~3) + disp)
    if not any(values_equivalent(expected, asm_target.get("imm", 0), 0, asm_target.get("width", 0)) for expected in candidates):
        return False
    cap_without_mem = [op for op in cap_ops if op is not cap_mem]
    asm_without_target = [op for op in asm_ops if op is not asm_target]
    return atom_counters_match_allow_extra_detail(arch, operand_atoms(arch, cap_without_mem, "capstone"), operand_atoms(arch, asm_without_target, "asmkit"))


def normalized_atom_for_match(arch: str, atom):
    if arch == "arm64" and atom[0] in {"reg", "mem-index"}:
        match = re.fullmatch(r"[bhsdqv]([0-9]+)", str(atom[1]))
        if match is not None:
            return (atom[0], f"v{match.group(1)}")
    return atom


def normalize_atoms_for_match(arch: str, atoms: Counter) -> Counter:
    out = Counter()
    for atom, count in atoms.items():
        out[normalized_atom_for_match(arch, atom)] += count
    return out


def atom_counters_match_allow_extra_detail(arch: str, cap_atoms: Counter, asm_atoms: Counter) -> bool:
    cap_atoms = normalize_atoms_for_match(arch, cap_atoms)
    asm_atoms = normalize_atoms_for_match(arch, asm_atoms)
    if cap_atoms == asm_atoms:
        return True
    missing = cap_atoms - asm_atoms
    if missing:
        return False
    extra = asm_atoms - cap_atoms
    for atom, count in extra.items():
        if count <= 0:
            continue
        if atom[0] == "mem-base" and cap_atoms.get(("reg", atom[1]), 0) >= count:
            continue
        if atom[0] == "reg" and cap_atoms.get(("mem-base", atom[1]), 0) >= count:
            continue
        return False
    return True


def a64_reg_family_equivalent(left: str, right: str) -> bool:
    if len(left) < 2 or len(right) < 2:
        return left == right
    if left[0] not in {"w", "x"} or right[0] not in {"w", "x"}:
        return left == right
    return left[1:] == right[1:]


def a64_reg_semantic_equivalent(left: str, right: str) -> bool:
    return left == right or a64_reg_family_equivalent(left, right) or a64_vector_reg_equivalent(left, right)


def a64_alias_regs_family_equivalent(left_ops, right_ops) -> bool:
    if len(left_ops) != len(right_ops):
        return False
    for left, right in zip(left_ops, right_ops):
        if left["kind"] != right["kind"]:
            return False
        if left["kind"] == "reg":
            if not a64_reg_semantic_equivalent(left.get("reg", ""), right.get("reg", "")):
                return False
            continue
        if not operand_value_equal(left, right):
            return False
    return True


def compare_operand_details(arch: str, cap_insn, asm_item, reason: str):
    cap_mnemonic = normalize_mnemonic(arch, cap_insn.mnemonic)
    asm_mnemonic = normalize_mnemonic(arch, asmkit_mnemonic_from_text(arch, asm_item["text"]))
    cap_ops = capstone_operands(arch, cap_insn)
    asm_ops = asmkit_operands(arch, asm_item)
    warnings = []

    if reason in {"exact", "x86-notrack-prefix"}:
        if arch in {"x86", "amd64"}:
            x86_base = x86_strip_prefix_words(cap_mnemonic)
            if x86_base in X86_STRING_MNEMONICS:
                cap_atoms = operand_atoms(arch, cap_ops, "capstone")
                asm_atoms = operand_atoms(arch, asm_ops, "asmkit") + asmkit_register_effect_atoms(arch, asm_item)
                if not (cap_atoms - asm_atoms):
                    return None, warnings
            cap_atoms = operand_atoms(arch, cap_ops, "capstone")
            asm_atoms_with_effects = operand_atoms(arch, asm_ops, "asmkit") + asmkit_register_effect_atoms(arch, asm_item)
            missing = cap_atoms - asm_atoms_with_effects
            if missing and set(missing.keys()) == {("reg", "st0")}:
                return None, warnings
            if not missing and cap_atoms != operand_atoms(arch, asm_ops, "asmkit"):
                warnings.append({
                    "kind": "capstone-explicit-operands-covered-by-asmkit-effects",
                    "capstone": f"{cap_insn.mnemonic} {cap_insn.op_str}".strip(),
                    "asmkit": asm_item["text"],
                })
                return None, warnings
        if arch in {"arm", "arm64"} and cap_mnemonic in {"dmb", "dsb", "isb"} and not cap_ops and arm_barrier_operands_match(cap_insn, asm_ops):
            return None, warnings
        if arch == "arm" and arm_it_following_instruction_count(cap_mnemonic) != 0 and not cap_ops and arm_it_operands_match(cap_insn, asm_ops):
            return None, warnings
        if arch == "arm" and cap_mnemonic in {"cpsie", "cpsid"} and not cap_ops and arm_cps_operands_match(cap_mnemonic, cap_insn, asm_ops):
            return None, warnings
        if arch == "arm" and arm_vfp_fpimm_encoding_equivalent(cap_mnemonic, cap_ops, asm_ops):
            warnings.append({
                "kind": "asmkit-arm-vfp-fpimm-encoding",
                "capstone": f"{cap_insn.mnemonic} {cap_insn.op_str}".strip(),
                "asmkit": asm_item["text"],
                "asmkit_immediate": asm_ops[1].get("imm", 0),
            })
            return None, warnings
        if arch == "arm" and arm_vcvt_fixed_point_immediate_equivalent(cap_mnemonic, cap_ops, asm_ops):
            warnings.append({
                "kind": "asmkit-arm-vcvt-fixed-point-immediate",
                "capstone": f"{cap_insn.mnemonic} {cap_insn.op_str}".strip(),
                "asmkit": asm_item["text"],
                "asmkit_immediate": asm_ops[1].get("imm", 0),
            })
            return None, warnings
        if arch == "arm" and arm_coprocessor_apsr_alias_equivalent(cap_mnemonic, cap_ops, asm_ops):
            warnings.append({
                "kind": "capstone-arm-coprocessor-apsr-alias",
                "capstone": f"{cap_insn.mnemonic} {cap_insn.op_str}".strip(),
                "asmkit": asm_item["text"],
            })
            return None, warnings
        if arch == "arm" and arm_extend_rotate_encoding_equivalent(cap_insn, cap_mnemonic, cap_ops, asm_ops):
            warnings.append({
                "kind": "asmkit-arm-extend-rotate-encoding",
                "capstone": f"{cap_insn.mnemonic} {cap_insn.op_str}".strip(),
                "asmkit": asm_item["text"],
            })
            return None, warnings
        if arch == "arm" and arm_vector_memory_detail_equivalent(cap_insn, cap_mnemonic, cap_ops, asm_ops):
            warnings.append({
                "kind": "asmkit-arm-vector-memory-detail",
                "capstone": f"{cap_insn.mnemonic} {cap_insn.op_str}".strip(),
                "asmkit": asm_item["text"],
            })
            return None, warnings
        if arch == "arm" and cap_mnemonic == "msr" and asm_mnemonic == "msr" and arm_msr_operands_match(cap_ops, asm_ops):
            warnings.append({
                "kind": "capstone-msr-mask-covered-by-asmkit",
                "capstone": f"{cap_insn.mnemonic} {cap_insn.op_str}".strip(),
                "asmkit": asm_item["text"],
            })
            return None, warnings
        if arch == "arm64" and cap_mnemonic in {"movk", "movn", "movz"} and len(cap_ops) == 2 and len(asm_ops) == 3 and asm_ops[2]["kind"] == "imm":
            if comparable_sequence(cap_ops, asm_ops[:2]) and asm_ops[2].get("imm", 0) == capstone_lsl_shift_amount(cap_insn.op_str):
                return None, warnings
        if arch == "arm64" and cap_mnemonic in {"add", "adds", "sub", "subs"} and len(cap_ops) == 3 and len(asm_ops) == 4 and asm_ops[2]["kind"] == "imm" and asm_ops[3]["kind"] == "imm":
            shifted_ops = [asm_ops[0], asm_ops[1], dict(asm_ops[2])]
            shifted_ops[2]["imm"] = shifted_ops[2].get("imm", 0) << asm_ops[3].get("imm", 0)
            if comparable_sequence(cap_ops, shifted_ops):
                return None, warnings
        if arch == "arm64" and cap_mnemonic.startswith("b.") and asm_mnemonic.startswith("b.") and len(asm_ops) >= 2:
            cond = A64_CONDITIONS.get(cap_mnemonic.split(".", 1)[1])
            if cond is not None and asm_ops[0]["kind"] == "imm" and values_equivalent(asm_ops[0].get("imm", -1), cond, asm_ops[0].get("width", 0), 0):
                asm_ops = asm_ops[1:]
        if arch == "arm64" and cap_mnemonic == "ret" and asm_mnemonic == "ret" and not cap_ops and len(asm_ops) == 1 and asm_ops[0]["kind"] == "reg" and asm_ops[0].get("reg") == "x30":
            asm_ops = []
        if arch == "arm64" and cap_mnemonic == "bti" and not cap_ops and len(asm_ops) == 1 and asm_ops[0]["kind"] == "imm":
            warnings.append({
                "kind": "asmkit-aarch64-bti-hint-immediate",
                "capstone": f"{cap_insn.mnemonic} {cap_insn.op_str}".strip(),
                "asmkit": asm_item["text"],
                "asmkit_immediate": asm_ops[0].get("imm", 0),
            })
            return None, warnings
        if arch == "arm64" and cap_mnemonic in A64_CONDITION_OPERAND_MNEMONICS and asm_ops and asm_ops[-1]["kind"] == "imm":
            condition_text = cap_insn.op_str.rsplit(",", 1)[-1].strip().lower() if "," in cap_insn.op_str else ""
            cond = A64_CONDITIONS.get(condition_text)
            if cond is not None and values_equivalent(asm_ops[-1].get("imm", -1), cond, asm_ops[-1].get("width", 0), 0):
                asm_ops = asm_ops[:-1]
        if arch == "arm64" and a64_fpimm_encoding_equivalent(cap_mnemonic, cap_ops, asm_ops):
            warnings.append({
                "kind": "asmkit-aarch64-fpimm-encoding",
                "capstone": f"{cap_insn.mnemonic} {cap_insn.op_str}".strip(),
                "asmkit": asm_item["text"],
                "asmkit_immediate": asm_ops[1].get("imm", 0),
            })
            return None, warnings
        if arch == "arm64" and a64_vector_imm_encoding_equivalent(cap_mnemonic, cap_ops, asm_ops):
            warnings.append({
                "kind": "asmkit-aarch64-vector-imm-encoding",
                "capstone": f"{cap_insn.mnemonic} {cap_insn.op_str}".strip(),
                "asmkit": asm_item["text"],
                "asmkit_immediate": asm_ops[1].get("imm", 0),
            })
            return None, warnings
        if arch == "arm64" and a64_vector_extra_immediate_equivalent(cap_mnemonic, cap_ops, asm_ops):
            warnings.append({
                "kind": "asmkit-aarch64-vector-extra-immediate",
                "capstone": f"{cap_insn.mnemonic} {cap_insn.op_str}".strip(),
                "asmkit": asm_item["text"],
                "asmkit_extra_immediates": [op.get("imm", 0) for op in asm_ops[len(cap_ops):]],
            })
            return None, warnings
        if arch == "arm64" and a64_opaque_immediate_encoding_equivalent(cap_mnemonic, cap_ops, asm_ops):
            warnings.append({
                "kind": "asmkit-aarch64-opaque-immediate-encoding",
                "capstone": f"{cap_insn.mnemonic} {cap_insn.op_str}".strip(),
                "asmkit": asm_item["text"],
            })
            return None, warnings
        if arch == "arm64" and a64_prfm_register_offset_detail_equivalent(cap_mnemonic, asm_mnemonic, cap_ops, asm_ops):
            warnings.append({
                "kind": "asmkit-aarch64-prfm-register-offset-detail",
                "capstone": f"{cap_insn.mnemonic} {cap_insn.op_str}".strip(),
                "asmkit": asm_item["text"],
            })
            return None, warnings
        if arch == "arm64" and a64_vector_memory_detail_equivalent(cap_insn, cap_ops, asm_ops):
            warnings.append({
                "kind": "capstone-aarch64-vector-memory-detail-missing",
                "capstone": f"{cap_insn.mnemonic} {cap_insn.op_str}".strip(),
                "asmkit": asm_item["text"],
            })
            return None, warnings
        if arch == "arm64" and a64_sve_default_immediate_operands_equivalent(cap_insn, cap_mnemonic, asm_mnemonic, cap_ops, asm_ops):
            warnings.append({
                "kind": "asmkit-aarch64-sve-default-immediates",
                "capstone": f"{cap_insn.mnemonic} {cap_insn.op_str}".strip(),
                "asmkit": asm_item["text"],
                "asmkit_extra_immediates": [op.get("imm", 0) for op in asm_ops if op.get("kind") == "imm"],
            })
            return None, warnings
        if arch == "arm64" and a64_sve_incdec_default_operands_equivalent(cap_mnemonic, asm_mnemonic, cap_ops, asm_ops):
            warnings.append({
                "kind": "asmkit-aarch64-sve-incdec-default-operands",
                "capstone": f"{cap_insn.mnemonic} {cap_insn.op_str}".strip(),
                "asmkit": asm_item["text"],
                "asmkit_default_immediates": [asm_ops[-2].get("imm", 0), asm_ops[-1].get("imm", 0)],
            })
            return None, warnings
        if arch == "arm64" and a64_tied_readwrite_operand_equivalent(cap_ops, asm_ops):
            warnings.append({
                "kind": "asmkit-aarch64-tied-readwrite-operand",
                "capstone": f"{cap_insn.mnemonic} {cap_insn.op_str}".strip(),
                "asmkit": asm_item["text"],
            })
            return None, warnings
        if arch == "arm" and tied_readwrite_operand_equivalent("arm", cap_ops, asm_ops):
            warnings.append({
                "kind": "asmkit-arm-tied-readwrite-operand",
                "capstone": f"{cap_insn.mnemonic} {cap_insn.op_str}".strip(),
                "asmkit": asm_item["text"],
            })
            return None, warnings
        if arch == "arm" and arm_postindex_register_detail_equivalent(cap_insn, cap_ops, asm_ops):
            warnings.append({
                "kind": "asmkit-arm-postindex-register-detail",
                "capstone": f"{cap_insn.mnemonic} {cap_insn.op_str}".strip(),
                "asmkit": asm_item["text"],
            })
            return None, warnings
        if arch == "arm" and arm_postindex_immediate_detail_equivalent(cap_insn, cap_ops, asm_ops):
            warnings.append({
                "kind": "asmkit-arm-postindex-immediate-detail",
                "capstone": f"{cap_insn.mnemonic} {cap_insn.op_str}".strip(),
                "asmkit": asm_item["text"],
            })
            return None, warnings
        if arch == "arm" and arm_vector_alignment_detail_equivalent(cap_insn, cap_mnemonic, cap_ops, asm_ops):
            warnings.append({
                "kind": "asmkit-arm-neon-alignment-detail",
                "capstone": f"{cap_insn.mnemonic} {cap_insn.op_str}".strip(),
                "asmkit": asm_item["text"],
            })
            return None, warnings
        if arch == "arm" and arm_vector_lane_immediate_equivalent(cap_insn, cap_ops, asm_ops):
            warnings.append({
                "kind": "asmkit-arm-vector-lane-immediate",
                "capstone": f"{cap_insn.mnemonic} {cap_insn.op_str}".strip(),
                "asmkit": asm_item["text"],
            })
            return None, warnings
        if arch == "arm64" and a64_sme_za_capstone_partial_equivalent(cap_insn, cap_ops, asm_ops):
            warnings.append({
                "kind": "capstone-aarch64-sme-za-operand-not-expanded",
                "capstone": f"{cap_insn.mnemonic} {cap_insn.op_str}".strip(),
                "asmkit": asm_item["text"],
            })
            return None, warnings
        if arch == "arm64" and a64_vector_lane_imm_equivalent(cap_ops, asm_ops):
            return None, warnings
        if arch in {"arm", "arm64"} and pc_relative_target_matches(arch, asm_item.get("mode", ""), cap_insn, cap_ops, asm_ops):
            return None, warnings
        if default_zero_imm_equivalent(cap_mnemonic, asm_mnemonic, cap_ops, asm_ops):
            return None, warnings
        if implicit_fixed_operand_equivalent(arch, cap_mnemonic, cap_ops, asm_ops):
            return None, warnings
        if comparable_sequence(cap_ops, asm_ops):
            return None, warnings
        cap_atoms = operand_atoms(arch, cap_ops, "capstone")
        asm_atoms = operand_atoms(arch, asm_ops, "asmkit")
        if atom_counters_match_allow_extra_detail(arch, cap_atoms, asm_atoms):
            return None, warnings
        return {
            "kind": "operand",
            "capstone_operands": cap_ops,
            "asmkit_operands": asm_ops,
            "capstone_atoms": sorted(cap_atoms.items()),
            "asmkit_atoms": sorted(asm_atoms.items()),
        }, warnings

    filtered = None
    if arch == "arm" and arm_postindex_register_detail_equivalent(cap_insn, cap_ops, asm_ops):
        warnings.append({
            "kind": "asmkit-arm-postindex-register-detail",
            "capstone": f"{cap_insn.mnemonic} {cap_insn.op_str}".strip(),
            "asmkit": asm_item["text"],
            "alias_rule": reason,
        })
        return None, warnings
    if arch == "arm64":
        filtered = filtered_a64_ops_for_alias(cap_mnemonic, asm_mnemonic, cap_ops, asm_ops)
    elif arch == "arm":
        filtered = filtered_arm_ops_for_alias(cap_mnemonic, asm_mnemonic, cap_ops, asm_ops)
    if filtered is None:
        warnings.append({
            "kind": "alias-operands-not-fully-modeled",
            "capstone": f"{cap_insn.mnemonic} {cap_insn.op_str}".strip(),
            "asmkit": asm_item["text"],
            "reason": reason,
            "capstone_operands": cap_ops,
            "asmkit_operands": asm_ops,
        })
        return None, warnings

    expected_cap, expected_asm, alias_rule = filtered
    if arch == "arm" and arm_vfp_fpimm_encoding_equivalent(cap_mnemonic, expected_cap, expected_asm):
        warnings.append({
            "kind": "asmkit-arm-vfp-fpimm-encoding",
            "capstone": f"{cap_insn.mnemonic} {cap_insn.op_str}".strip(),
            "asmkit": asm_item["text"],
            "alias_rule": alias_rule,
            "asmkit_immediate": expected_asm[1].get("imm", 0),
        })
        return None, warnings
    if arch == "arm" and arm_vcvt_fixed_point_immediate_equivalent(cap_mnemonic, expected_cap, expected_asm):
        warnings.append({
            "kind": "asmkit-arm-vcvt-fixed-point-immediate",
            "capstone": f"{cap_insn.mnemonic} {cap_insn.op_str}".strip(),
            "asmkit": asm_item["text"],
            "alias_rule": alias_rule,
            "asmkit_immediate": expected_asm[1].get("imm", 0),
        })
        return None, warnings
    if arch == "arm" and arm_coprocessor_apsr_alias_equivalent(cap_mnemonic, expected_cap, expected_asm):
        warnings.append({
            "kind": "capstone-arm-coprocessor-apsr-alias",
            "capstone": f"{cap_insn.mnemonic} {cap_insn.op_str}".strip(),
            "asmkit": asm_item["text"],
            "alias_rule": alias_rule,
        })
        return None, warnings
    if arch == "arm" and arm_extend_rotate_encoding_equivalent(cap_insn, cap_mnemonic, expected_cap, expected_asm):
        warnings.append({
            "kind": "asmkit-arm-extend-rotate-encoding",
            "capstone": f"{cap_insn.mnemonic} {cap_insn.op_str}".strip(),
            "asmkit": asm_item["text"],
            "alias_rule": alias_rule,
        })
        return None, warnings
    if arch == "arm" and arm_vector_memory_detail_equivalent(cap_insn, cap_mnemonic, expected_cap, expected_asm):
        warnings.append({
            "kind": "asmkit-arm-vector-memory-detail",
            "capstone": f"{cap_insn.mnemonic} {cap_insn.op_str}".strip(),
            "asmkit": asm_item["text"],
            "alias_rule": alias_rule,
        })
        return None, warnings
    if comparable_sequence(expected_cap, expected_asm):
        return None, warnings
    if arch == "arm64" and a64_alias_regs_family_equivalent(expected_cap, expected_asm):
        return None, warnings
    if arch in {"arm", "arm64"} and tied_readwrite_operand_equivalent(arch, expected_cap, expected_asm):
        warnings.append({
            "kind": f"asmkit-{arch}-tied-readwrite-operand",
            "capstone": f"{cap_insn.mnemonic} {cap_insn.op_str}".strip(),
            "asmkit": asm_item["text"],
            "alias_rule": alias_rule,
        })
        return None, warnings
    if arch == "arm" and arm_postindex_register_detail_equivalent(cap_insn, expected_cap, expected_asm):
        warnings.append({
            "kind": "asmkit-arm-postindex-register-detail",
            "capstone": f"{cap_insn.mnemonic} {cap_insn.op_str}".strip(),
            "asmkit": asm_item["text"],
            "alias_rule": alias_rule,
        })
        return None, warnings
    if arch == "arm" and arm_postindex_immediate_detail_equivalent(cap_insn, expected_cap, expected_asm):
        warnings.append({
            "kind": "asmkit-arm-postindex-immediate-detail",
            "capstone": f"{cap_insn.mnemonic} {cap_insn.op_str}".strip(),
            "asmkit": asm_item["text"],
            "alias_rule": alias_rule,
        })
        return None, warnings
    if arch == "arm" and arm_vector_lane_immediate_equivalent(cap_insn, expected_cap, expected_asm):
        warnings.append({
            "kind": "asmkit-arm-vector-lane-immediate",
            "capstone": f"{cap_insn.mnemonic} {cap_insn.op_str}".strip(),
            "asmkit": asm_item["text"],
            "alias_rule": alias_rule,
        })
        return None, warnings
    if atom_counters_match_allow_extra_detail(arch, operand_atoms(arch, expected_cap, "capstone"), operand_atoms(arch, expected_asm, "asmkit")):
        return None, warnings
    return {
        "kind": "operand",
        "alias_rule": alias_rule,
        "capstone_operands": cap_ops,
        "asmkit_operands": asm_ops,
        "compared_capstone_operands": expected_cap,
        "compared_asmkit_operands": expected_asm,
    }, warnings


def compare_semantic_flags(arch: str, insn, asm_item, arm_in_it_block: bool = False):
    semantics = capstone_insn_semantics(arch, insn)
    flags = asm_item["flags_value"]
    attrs = asm_item["attributes_value"]
    inst_class = asm_item["class"]
    issues = []

    def has_flag(name: str) -> bool:
        return (flags & ASMKIT_INST_FLAGS[name]) != 0

    def has_attr(name: str) -> bool:
        return (attrs & ASMKIT_INSTRUCTION_ATTRS[name]) != 0

    branch_classes = {4, 5, 6}
    call_classes = {7, 8}
    if semantics["branch"] and inst_class not in branch_classes:
        issues.append(f"expected branch class, got {ASMKIT_INST_CLASS_NAMES.get(inst_class, inst_class)}")
    if semantics["call"] and inst_class not in call_classes:
        issues.append(f"expected call class, got {ASMKIT_INST_CLASS_NAMES.get(inst_class, inst_class)}")
    if semantics["return"] and inst_class != 9:
        issues.append(f"expected return class, got {ASMKIT_INST_CLASS_NAMES.get(inst_class, inst_class)}")
    if semantics["conditional"] and not has_flag("conditional") and not has_attr("conditional") and not (arch == "arm" and arm_in_it_block):
        issues.append("missing conditional flag/attribute")
    if semantics["call"] and not has_flag("call") and not has_attr("call"):
        issues.append("missing call flag/attribute")
    if semantics["return"] and not has_flag("return") and not has_attr("return"):
        issues.append("missing return flag/attribute")
    if (semantics["branch"] or semantics["call"]) and semantics["direct"] and not has_flag("direct"):
        issues.append("missing direct flag")
    if (semantics["branch"] or semantics["call"]) and semantics["direct"] and any(op["kind"] == "target" for op in capstone_operands(arch, insn)):
        if not has_flag("pc-relative") and not has_attr("pc-relative"):
            issues.append("missing pc-relative flag/attribute")
    if arch in {"x86", "amd64"}:
        cap_text = f"{insn.mnemonic} {insn.op_str}".strip().lower()
        if cap_text.startswith("lock ") and not has_flag("x86-lock"):
            issues.append("missing x86 lock flag")
        if cap_text.startswith(("rep ", "repe ", "repz ")) and not has_flag("x86-rep"):
            issues.append("missing x86 rep flag")
        if cap_text.startswith(("repne ", "repnz ")) and not has_flag("x86-repne"):
            issues.append("missing x86 repne flag")
    if not issues:
        return None
    return {
        "kind": "semantic-flags",
        "capstone_semantics": semantics,
        "asmkit_class": ASMKIT_INST_CLASS_NAMES.get(inst_class, str(inst_class)),
        "asmkit_flags": hex(flags),
        "asmkit_attributes": hex(attrs),
        "issues": issues,
    }


def compare_register_effects(arch: str, insn, asm_item):
    cap_reads = {normalize_register_name(arch, insn.reg_name(reg)) for reg in getattr(insn, "regs_read", [])}
    cap_writes = {normalize_register_name(arch, insn.reg_name(reg)) for reg in getattr(insn, "regs_write", [])}
    cap_reads.discard("")
    cap_writes.discard("")
    if not cap_reads and not cap_writes:
        return None
    asm_effects = asm_item.get("effects_parsed", [])
    asm_reads = {e["reg_name"] for e in asm_effects if (e["flags"] & ASMKIT_REG_EFFECT_FLAGS["read"]) != 0}
    asm_writes = {e["reg_name"] for e in asm_effects if (e["flags"] & ASMKIT_REG_EFFECT_FLAGS["write"]) != 0}
    operand_regs_read = set()
    operand_regs_write = set()
    for op in asm_item.get("operands_normalized", []):
        if op.get("kind") in {"reg", "mem-base"}:
            if op.get("access", 0) & 1:
                operand_regs_read.add(op.get("reg", ""))
            if op.get("access", 0) & 2:
                operand_regs_write.add(op.get("reg", ""))
    missing_reads = sorted(cap_reads - asm_reads - operand_regs_read)
    missing_writes = sorted(cap_writes - asm_writes - operand_regs_write)
    if not missing_reads and not missing_writes:
        return None
    return {
        "kind": "register-effects",
        "capstone_reads": sorted(cap_reads),
        "capstone_writes": sorted(cap_writes),
        "asmkit_effect_reads": sorted(asm_reads),
        "asmkit_effect_writes": sorted(asm_writes),
        "missing_reads": missing_reads,
        "missing_writes": missing_writes,
    }


def compare_operand_access(arch: str, insn, asm_item):
    cap_ops = capstone_operands(arch, insn)
    asm_ops = asm_item.get("operands_normalized")
    if asm_ops is None:
        asm_ops = asmkit_operands(arch, asm_item)
    cap_reads = operand_access_atoms(arch, cap_ops, 1)
    cap_writes = operand_access_atoms(arch, cap_ops, 2)
    asm_reads = operand_access_atoms(arch, asm_ops, 1)
    asm_writes = operand_access_atoms(arch, asm_ops, 2)
    if not cap_reads and not cap_writes:
        return None
    missing_reads = cap_reads - asm_reads
    missing_writes = cap_writes - asm_writes
    mnemonic = normalize_mnemonic(arch, insn.mnemonic)
    if arch in {"arm", "arm64"} and mnemonic.startswith(("ldr", "ldrsw", "prfm")):
        has_pc_literal = any(op.get("kind") == "mem" and op.get("base") == "pc" for op in cap_ops)
        has_target = any(op.get("kind") == "target" and (op.get("access", 0) & 1) != 0 for op in asm_ops)
        if has_pc_literal and has_target:
            missing_reads = Counter({atom: count for atom, count in missing_reads.items() if atom[0] != "mem"})
    if not missing_reads and not missing_writes:
        return None
    return {
        "kind": "operand-access",
        "capstone_reads": sorted([[list(atom), count] for atom, count in cap_reads.items()]),
        "capstone_writes": sorted([[list(atom), count] for atom, count in cap_writes.items()]),
        "asmkit_reads": sorted([[list(atom), count] for atom, count in asm_reads.items()]),
        "asmkit_writes": sorted([[list(atom), count] for atom, count in asm_writes.items()]),
        "missing_reads": sorted([[list(atom), count] for atom, count in missing_reads.items()]),
        "missing_writes": sorted([[list(atom), count] for atom, count in missing_writes.items()]),
    }


def processor_flag_register_names(arch: str) -> set[str]:
    if arch in {"x86", "amd64"}:
        return {"flags"}
    if arch == "arm64":
        return {"nzcv"}
    if arch == "arm":
        return {"apsr", "cpsr", "spsr", "fpscr", "fpexc"}
    return set()


def processor_flag_register_canonical(arch: str, name: str) -> str:
    name = normalize_register_name(arch, name)
    if arch == "arm" and name in {"apsr", "apsr_nzcv", "cpsr"}:
        return "apsr"
    return name


def capstone_processor_flag_detail(arch: str, insn) -> dict:
    detail = {}
    for name in ("eflags", "fpu_flags", "update_flags", "cc", "cps_mode", "cps_flag"):
        if hasattr(insn, name):
            try:
                value = getattr(insn, name)
            except Exception:
                continue
            if isinstance(value, int):
                detail[name] = value
            elif isinstance(value, bool):
                detail[name] = bool(value)
    return detail


def compare_processor_flags(arch: str, insn, asm_item, arm_in_it_block: bool = False):
    flag_regs = processor_flag_register_names(arch)
    if not flag_regs:
        return None
    cap_detail = capstone_processor_flag_detail(arch, insn)
    cap_reads = {processor_flag_register_canonical(arch, insn.reg_name(reg)) for reg in getattr(insn, "regs_read", [])}
    cap_writes = {processor_flag_register_canonical(arch, insn.reg_name(reg)) for reg in getattr(insn, "regs_write", [])}
    cap_reads &= flag_regs
    cap_writes &= flag_regs
    if arch == "arm64" and cap_detail.get("update_flags"):
        cap_writes.add("nzcv")
    if arch == "arm" and cap_detail.get("update_flags"):
        cap_writes.add("apsr")
    if arch == "arm" and cap_detail.get("cc", 15) not in {0, 15} and arm_it_following_instruction_count(insn.mnemonic) == 0 and not arm_in_it_block:
        cap_reads.add("apsr")
    asm_effects = asm_item.get("effects_parsed", [])
    asm_reads = {
        processor_flag_register_canonical(arch, effect.get("reg_name", ""))
        for effect in asm_effects
        if (effect["flags"] & ASMKIT_REG_EFFECT_FLAGS["read"]) != 0
    } & flag_regs
    asm_writes = {
        processor_flag_register_canonical(arch, effect.get("reg_name", ""))
        for effect in asm_effects
        if (effect["flags"] & ASMKIT_REG_EFFECT_FLAGS["write"]) != 0
    } & flag_regs
    if arch == "arm" and (asm_item.get("flags_value", 0) & ASMKIT_INST_FLAGS["conditional"]) != 0:
        asm_reads.add("apsr")
    if not cap_reads and not cap_writes and not cap_detail:
        return None
    missing_reads = sorted(cap_reads - asm_reads)
    missing_writes = sorted(cap_writes - asm_writes)
    if not missing_reads and not missing_writes:
        return None
    return {
        "kind": "processor-flags",
        "capstone_detail": cap_detail,
        "capstone_flag_reads": sorted(cap_reads),
        "capstone_flag_writes": sorted(cap_writes),
        "asmkit_flag_reads": sorted(asm_reads),
        "asmkit_flag_writes": sorted(asm_writes),
        "missing_reads": missing_reads,
        "missing_writes": missing_writes,
    }


def detail_kind_counts(items: list[dict], key: str = "kind") -> dict:
    counts = Counter()
    for item in items:
        counts[item.get(key, "unknown")] += 1
    return dict(sorted(counts.items()))


def llvm_record_matches_asmkit(llvm_item, asm_item) -> bool:
    if llvm_item is None:
        return False
    llvm_name = str(llvm_item.get("llvm_name", "")).lower()
    asm_llvm_name = str(asm_item.get("llvm_name", "")).lower()
    asm_name = str(asm_item.get("asmkit_name", "")).lower()
    record_aliases = {
        ("movpclr", "movr"),
    }
    if (llvm_name, asm_llvm_name) in record_aliases or (llvm_name, asm_name) in record_aliases:
        return True
    return bool(llvm_name) and (llvm_name == asm_llvm_name or llvm_name == asm_name)


def llvm_mc_immediate_values(llvm_item) -> set[int]:
    values = set()
    if llvm_item is None:
        return values
    for operand in llvm_item.get("mc_operands", []):
        if str(operand.get("kind", "")).lower() != "imm":
            continue
        text = str(operand.get("value", "")).strip()
        if not text:
            continue
        try:
            values.add(int(text, 0))
        except ValueError:
            continue
    return values


def llvm_mnemonic_matches(arch: str, llvm_item, cap_mnemonic: str, asm_mnemonic: str) -> bool:
    if llvm_item is None:
        return False
    llvm_mnemonic = llvm_item.get("mnemonic", "")
    cap_ok, _cap_reason = mnemonics_equivalent(arch, cap_mnemonic, llvm_mnemonic)
    asm_ok, _asm_reason = mnemonics_equivalent(arch, llvm_mnemonic, asm_mnemonic)
    return cap_ok or asm_ok


def resolve_detail_diagnostic(arch: str, warning: dict, cap_insn, asm_item, llvm_item):
    asm_mnemonic = asmkit_mnemonic_from_text(arch, asm_item["text"])
    if llvm_item is None:
        return None, {
            **warning,
            "kind": "unresolved-detail-without-llvm",
            "detail_kind": warning.get("kind", "unknown"),
            "reason": "LLVM MC output is unavailable for this instruction",
        }
    if arch == "arm64" and warning.get("kind") == "asmkit-aarch64-bti-hint-immediate":
        asm_imm = int(warning.get("asmkit_immediate", -1))
        if str(llvm_item.get("llvm_name", "")).lower() == "hint" and asm_imm in llvm_mc_immediate_values(llvm_item):
            return {
                "kind": warning.get("kind", "unknown"),
                "resolution": "llvm-hint-immediate-matches-asmkit",
                "llvm": {
                    "text": llvm_item.get("text", ""),
                    "llvm_name": llvm_item.get("llvm_name", ""),
                    "mc_id": llvm_item.get("mc_id", 0),
                    "mc_operands": llvm_item.get("mc_operands", []),
                },
                **warning,
            }, None
    if arch == "arm64" and warning.get("kind") in {"asmkit-aarch64-fpimm-encoding", "asmkit-aarch64-vector-extra-immediate", "asmkit-aarch64-vector-imm-encoding"}:
        if llvm_record_matches_asmkit(llvm_item, asm_item) or llvm_mnemonic_matches(arch, llvm_item, cap_insn.mnemonic, asm_mnemonic):
            return {
                "kind": warning.get("kind", "unknown"),
                "resolution": "llvm-confirms-asmkit-encoded-immediates",
                "llvm": {
                    "text": llvm_item.get("text", ""),
                    "llvm_name": llvm_item.get("llvm_name", ""),
                    "mc_id": llvm_item.get("mc_id", 0),
                    "mc_operands": llvm_item.get("mc_operands", []),
                },
                **warning,
            }, None
    if llvm_record_matches_asmkit(llvm_item, asm_item):
        return {
            "kind": warning.get("kind", "unknown"),
            "resolution": "llvm-record-matches-asmkit",
            "llvm": {
                "text": llvm_item.get("text", ""),
                "llvm_name": llvm_item.get("llvm_name", ""),
                "mc_id": llvm_item.get("mc_id", 0),
                "mc_operand_count": len(llvm_item.get("mc_operands", [])),
            },
            **warning,
        }, None
    if warning.get("kind") in {"alias-operands-not-fully-modeled"} and llvm_mnemonic_matches(arch, llvm_item, cap_insn.mnemonic, asm_mnemonic):
        return {
            "kind": warning.get("kind", "unknown"),
            "resolution": "llvm-mnemonic-alias-covered",
            "llvm": {
                "text": llvm_item.get("text", ""),
                "llvm_name": llvm_item.get("llvm_name", ""),
                "mc_id": llvm_item.get("mc_id", 0),
                "mc_operand_count": len(llvm_item.get("mc_operands", [])),
            },
            **warning,
        }, None
    return None, {
        **warning,
        "kind": "unresolved-detail-disagreement",
        "detail_kind": warning.get("kind", "unknown"),
        "llvm": {
            "text": llvm_item.get("text", ""),
            "mnemonic": llvm_item.get("mnemonic", ""),
            "llvm_name": llvm_item.get("llvm_name", ""),
            "mc_id": llvm_item.get("mc_id", 0),
            "mc_operands": llvm_item.get("mc_operands", []),
        },
        "asmkit_llvm_name": asm_item.get("llvm_name", ""),
        "asmkit_name": asm_item.get("asmkit_name", ""),
    }


def parse_asmkit_output(text: str):
    instructions = []
    errors = []
    for line in text.splitlines():
        fields = line.rstrip("\n").split("\t")
        if not fields:
            continue
        if fields[0] == "I" and len(fields) >= 13:
            item = {
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
                "effects": fields[13] if len(fields) >= 14 else "",
                "asmkit_name": fields[14] if len(fields) >= 15 else "",
                "llvm_name": fields[15] if len(fields) >= 16 else "",
            }
            item["flags_value"] = parse_int(item["flags"])
            item["attributes_value"] = parse_int(item["attributes"])
            item["operands_parsed"] = parse_asmkit_operands(item["operands"])
            item["effects_parsed"] = parse_asmkit_effects(item["effects"], "unknown")
            instructions.append(item)
        elif fields[0] == "E" and len(fields) >= 6:
            errors.append({
                "address": int(fields[1], 16),
                "offset": int(fields[2]),
                "status": fields[3],
                "code": int(fields[4]),
                "byte": fields[5],
            })
    return instructions, errors


def run_asmkit(helper: Path, arch: str, mode: str, base: int, code: bytes):
    helper_arch, helper_mode = asmkit_arch_mode(arch, mode)
    proc = subprocess.run(
        [str(helper), helper_arch, helper_mode, hex(base), code.hex()],
        cwd=str(Path.cwd()),
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        timeout=20,
    )
    instructions, errors = parse_asmkit_output(proc.stdout)
    for item in instructions:
        item["effects_parsed"] = parse_asmkit_effects(item.get("effects", ""), arch)
    return {
        "returncode": proc.returncode,
        "stdout": proc.stdout,
        "stderr": proc.stderr,
        "instructions": instructions,
        "errors": errors,
    }


def compare_function(
    helper: Path,
    llvm_mc: Path | None,
    arch: str,
    image,
    md,
    func: ExportFunction,
    max_instructions: int,
    max_detail_resolutions: int,
):
    base = image.image_base + func.rva
    code = image.bytes_at_rva(func.rva, func.max_size)
    cap_insns = capstone_instructions(md, code, base, max_instructions)
    consumed = instruction_consumed_size(cap_insns)
    if consumed == 0:
        return {
            "name": func.name,
            "source": func.source,
            "rva": func.rva,
            "mode": func.mode,
            "status": "skipped-no-capstone-instructions",
            "instruction_count": 0,
            "critical_mismatches": [],
            "alias_differences": [],
            "detail_resolutions": [],
        }
    code = code[:consumed]
    mode = func.mode or ""
    asm = run_asmkit(helper, arch, mode, base, code)
    llvm = run_llvm_mc(llvm_mc, arch, mode, code, len(cap_insns))
    critical = []
    aliases = []
    detail_resolutions = []
    llvm_stream_matches = False
    llvm_fatal_warning_count = 0
    llvm_stream_diagnostics = []
    if asm["returncode"] != 0:
        critical.append({"kind": "helper-returncode", "returncode": asm["returncode"], "stderr": asm["stderr"][-400:]})
    if llvm is not None:
        if llvm["returncode"] != 0:
            llvm_stream_diagnostics.append({"kind": "llvm-mc-returncode", "returncode": llvm["returncode"], "stderr": llvm["stderr"][-400:]})
        if llvm["warnings"]:
            benign_warnings = [w for w in llvm["warnings"] if "potentially undefined instruction encoding" in w]
            fatal_warnings = [w for w in llvm["warnings"] if w not in benign_warnings]
            llvm_fatal_warning_count = len(fatal_warnings)
            if benign_warnings:
                detail_resolutions.append({
                    "kind": "llvm-mc-warning",
                    "resolution": "llvm-disassembled-potentially-undefined-encoding",
                    "warnings": benign_warnings[:8],
                })
            if fatal_warnings:
                llvm_stream_diagnostics.append({"kind": "llvm-mc-warning", "args": llvm.get("args", []), "warnings": fatal_warnings[:8], "stderr": llvm["stderr"][-800:]})
        if len(llvm["instructions"]) != len(cap_insns):
            llvm_stream_diagnostics.append({
                "kind": "llvm-instruction-count",
                "capstone_count": len(cap_insns),
                "llvm_count": len(llvm["instructions"]),
                "llvm_args": llvm.get("args", []),
                "llvm_stdout": llvm["stdout"][-1200:],
                "llvm_stderr": llvm["stderr"][-800:],
            })
        else:
            llvm_stream_matches = llvm["returncode"] == 0 and llvm_fatal_warning_count == 0
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
    arm_it_remaining = 0
    single_llvm_cache = {}
    single_llvm_missing = []
    single_llvm_invalid_cache = {}

    def llvm_item_for_instruction(index: int, insn):
        if llvm_stream_matches:
            return llvm["instructions"][index] if llvm is not None and index < len(llvm["instructions"]) else None
        if llvm_mc is None:
            return None
        insn_bytes = bytes(insn.bytes)
        key = (arch, mode, insn_bytes)
        if key not in single_llvm_cache:
            single = run_llvm_mc(llvm_mc, arch, mode, insn_bytes, 1)
            item = None
            invalid = None
            if single is not None and single["returncode"] == 0 and len(single["instructions"]) == 1:
                fatal_warnings = [
                    line for line in single["warnings"]
                    if "potentially undefined instruction encoding" not in line
                ]
                if not fatal_warnings:
                    item = dict(single["instructions"][0])
                    item["args"] = single.get("args", [])
            elif single is not None and single["returncode"] == 0 and len(single["instructions"]) == 0:
                invalid_warnings = [
                    line for line in single["warnings"]
                    if "invalid instruction encoding" in line.lower()
                ]
                if invalid_warnings:
                    invalid = {
                        "args": single.get("args", []),
                        "warnings": invalid_warnings[:8],
                        "stderr": single["stderr"][-800:],
                    }
            single_llvm_cache[key] = item
            single_llvm_invalid_cache[key] = invalid
        return single_llvm_cache[key]

    def llvm_invalid_for_instruction(insn):
        if llvm_stream_matches or llvm_mc is None:
            return None
        key = (arch, mode, bytes(insn.bytes))
        if key not in single_llvm_cache:
            llvm_item_for_instruction(0, insn)
        return single_llvm_invalid_cache.get(key)

    for index, insn in enumerate(cap_insns):
        offset = int(insn.address - base)
        asm_item = asm_by_offset.get(offset)
        llvm_item = llvm_item_for_instruction(index, insn)
        llvm_invalid = llvm_invalid_for_instruction(insn)
        if not llvm_stream_matches and llvm_mc is not None and llvm_item is None and llvm_invalid is None:
            single_llvm_missing.append({
                "index": index,
                "offset": offset,
                "capstone": f"{insn.mnemonic} {insn.op_str}".strip(),
                "bytes": bytes(insn.bytes).hex(),
            })
        arm_in_it_block = arch == "arm" and arm_it_remaining > 0
        if asm_item is None:
            critical.append({
                "kind": "asmkit-missing-instruction",
                "index": index,
                "offset": offset,
                "capstone": f"{insn.mnemonic} {insn.op_str}".strip(),
                "bytes": bytes(insn.bytes).hex(),
            })
            if arch == "arm":
                if arm_it_remaining > 0:
                    arm_it_remaining -= 1
                arm_it_remaining = max(arm_it_remaining, arm_it_following_instruction_count(insn.mnemonic))
            continue
        asm_item["mode"] = mode
        if llvm_invalid is not None and normalize_mnemonic(arch, asmkit_mnemonic_from_text(arch, asm_item["text"])) in {"unknown", "other"}:
            detail_resolutions.append({
                "kind": "llvm-invalid-encoding",
                "resolution": "llvm-invalid-encoding-matches-asmkit-unknown",
                "index": index,
                "offset": offset,
                "capstone": f"{insn.mnemonic} {insn.op_str}".strip(),
                "asmkit": asm_item["text"],
                "bytes": bytes(insn.bytes).hex(),
                "llvm": llvm_invalid,
            })
            if arch == "arm":
                if arm_it_remaining > 0:
                    arm_it_remaining -= 1
                arm_it_remaining = max(arm_it_remaining, arm_it_following_instruction_count(insn.mnemonic))
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
            if arch == "arm":
                if arm_it_remaining > 0:
                    arm_it_remaining -= 1
                arm_it_remaining = max(arm_it_remaining, arm_it_following_instruction_count(insn.mnemonic))
            continue
        asm_mnemonic = asmkit_mnemonic_from_text(arch, asm_item["text"])
        equivalent, reason = mnemonics_equivalent(arch, insn.mnemonic, asm_mnemonic)
        if not equivalent and x86_notrack_prefix_equivalent(arch, insn.mnemonic, asm_mnemonic, asm_item):
            equivalent = True
            reason = "x86-notrack-prefix"
        if not equivalent and arch == "arm" and arm_in_it_block:
            cap_norm = normalize_mnemonic(arch, insn.mnemonic)
            asm_norm = normalize_mnemonic(arch, asm_mnemonic)
            for suffix in ARM_CONDITION_SUFFIXES:
                if cap_norm.endswith(suffix) and cap_norm[:-len(suffix)] in {"b", "bl"} and cap_norm[:-len(suffix)] == asm_norm:
                    equivalent = True
                    reason = "arm-it-conditional-branch"
                    break
        asm_item["operands_normalized"] = asmkit_operands(arch, asm_item)
        llvm_record_match = llvm_item is not None and llvm_record_matches_asmkit(llvm_item, asm_item)
        if llvm_item is not None and not llvm_record_match and not llvm_mnemonic_matches(arch, llvm_item, insn.mnemonic, asm_mnemonic):
            critical.append({
                "kind": "llvm-mnemonic",
                "index": index,
                "offset": offset,
                "capstone": f"{insn.mnemonic} {insn.op_str}".strip(),
                "asmkit": asm_item["text"],
                "llvm": llvm_item,
                "bytes": bytes(insn.bytes).hex(),
            })
        if llvm_item is not None and asm_item.get("llvm_name") and not llvm_record_match:
            critical.append({
                "kind": "llvm-record",
                "index": index,
                "offset": offset,
                "capstone": f"{insn.mnemonic} {insn.op_str}".strip(),
                "asmkit": asm_item["text"],
                "asmkit_name": asm_item.get("asmkit_name", ""),
                "asmkit_llvm_name": asm_item.get("llvm_name", ""),
                "llvm_name": llvm_item.get("llvm_name", ""),
                "llvm": llvm_item,
                "bytes": bytes(insn.bytes).hex(),
            })
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
            alias_entry = {
                "index": index,
                "offset": offset,
                "capstone": f"{insn.mnemonic} {insn.op_str}".strip(),
                "asmkit": asm_item["text"],
                "reason": reason,
                "bytes": bytes(insn.bytes).hex(),
            }
            aliases.append(alias_entry)
        if equivalent:
            operand_issue, operand_warnings = compare_operand_details(arch, insn, asm_item, reason)
            pending_warnings = [{
                "index": index,
                "offset": offset,
                "bytes": bytes(insn.bytes).hex(),
                **warning,
            } for warning in operand_warnings]
            for warning in pending_warnings:
                resolution, unresolved = resolve_detail_diagnostic(arch, warning, insn, asm_item, llvm_item)
                if resolution is not None:
                    detail_resolutions.append(resolution)
                if unresolved is not None:
                    critical.append(unresolved)
            if operand_issue is not None:
                critical.append({
                    "index": index,
                    "offset": offset,
                    "capstone": f"{insn.mnemonic} {insn.op_str}".strip(),
                    "asmkit": asm_item["text"],
                    "bytes": bytes(insn.bytes).hex(),
                    **operand_issue,
                })
        flag_issue = compare_semantic_flags(arch, insn, asm_item, arm_in_it_block)
        if flag_issue is not None:
            critical.append({
                "index": index,
                "offset": offset,
                "capstone": f"{insn.mnemonic} {insn.op_str}".strip(),
                "asmkit": asm_item["text"],
                "bytes": bytes(insn.bytes).hex(),
                **flag_issue,
            })
        effect_warning = compare_register_effects(arch, insn, asm_item)
        if effect_warning is not None:
            warning = {
                "index": index,
                "offset": offset,
                "capstone": f"{insn.mnemonic} {insn.op_str}".strip(),
                "asmkit": asm_item["text"],
                "bytes": bytes(insn.bytes).hex(),
                **effect_warning,
            }
            resolution, unresolved = resolve_detail_diagnostic(arch, warning, insn, asm_item, llvm_item)
            if resolution is not None:
                detail_resolutions.append(resolution)
            if unresolved is not None:
                critical.append(unresolved)
        access_warning = compare_operand_access(arch, insn, asm_item)
        if access_warning is not None:
            warning = {
                "index": index,
                "offset": offset,
                "capstone": f"{insn.mnemonic} {insn.op_str}".strip(),
                "asmkit": asm_item["text"],
                "bytes": bytes(insn.bytes).hex(),
                **access_warning,
            }
            resolution, unresolved = resolve_detail_diagnostic(arch, warning, insn, asm_item, llvm_item)
            if resolution is not None:
                detail_resolutions.append(resolution)
            if unresolved is not None:
                critical.append(unresolved)
        flag_warning = compare_processor_flags(arch, insn, asm_item, arm_in_it_block)
        if flag_warning is not None:
            warning = {
                "index": index,
                "offset": offset,
                "capstone": f"{insn.mnemonic} {insn.op_str}".strip(),
                "asmkit": asm_item["text"],
                "bytes": bytes(insn.bytes).hex(),
                **flag_warning,
            }
            resolution, unresolved = resolve_detail_diagnostic(arch, warning, insn, asm_item, llvm_item)
            if resolution is not None:
                detail_resolutions.append(resolution)
            if unresolved is not None:
                critical.append(unresolved)
        if asm_item["operand_count"] == 0 and insn.op_str:
            warning = {
                "kind": "asmkit-zero-operands-with-capstone-operands",
                "index": index,
                "offset": offset,
                "capstone": f"{insn.mnemonic} {insn.op_str}".strip(),
                "asmkit": asm_item["text"],
                "bytes": bytes(insn.bytes).hex(),
            }
            resolution, unresolved = resolve_detail_diagnostic(arch, warning, insn, asm_item, llvm_item)
            if resolution is not None:
                detail_resolutions.append(resolution)
            if unresolved is not None:
                critical.append(unresolved)
        if arch == "arm":
            if arm_it_remaining > 0:
                arm_it_remaining -= 1
            arm_it_remaining = max(arm_it_remaining, arm_it_following_instruction_count(insn.mnemonic))
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
    if llvm_stream_diagnostics:
        if llvm_mc is not None and not single_llvm_missing:
            detail_resolutions.append({
                "kind": "llvm-mc-stream-diagnostic",
                "resolution": "single-instruction-llvm-validation-used",
                "diagnostics": llvm_stream_diagnostics[:8],
            })
        else:
            critical.extend(llvm_stream_diagnostics)
    if single_llvm_missing:
        critical.append({
            "kind": "llvm-single-instruction-unavailable",
            "count": len(single_llvm_missing),
            "samples": single_llvm_missing[:8],
        })
    return {
        "name": func.name,
        "source": func.source,
        "export_rva": func.export_rva,
        "rva": func.rva,
        "address": base,
        "section": func.section,
        "mode": func.mode,
        "status": "ok" if not critical else "mismatch",
        "instruction_count": len(cap_insns),
        "byte_count": consumed,
        "critical_mismatches": critical,
        "alias_differences": aliases,
        "detail_resolution_count": len(detail_resolutions),
        "detail_resolution_kinds": detail_kind_counts(detail_resolutions),
        "detail_resolution_reasons": detail_kind_counts(detail_resolutions, "resolution"),
        "detail_resolutions": detail_resolutions if max_detail_resolutions <= 0 else detail_resolutions[:max_detail_resolutions],
    }


def select_samples(
    arch: str,
    image,
    candidates: list[ExportFunction],
    md_cache: dict,
    capstone_module,
    random_count: int,
    target_count: int,
    seed: int,
    max_instructions: int,
    all_functions: bool = False,
):
    eligible = []
    scored = []
    for func in candidates:
        code = image.bytes_at_rva(func.rva, func.max_size)
        md = capstone_decoder(md_cache, capstone_module, arch, func.mode)
        insns = capstone_instructions(md, code, image.image_base + func.rva, max_instructions)
        if not insns:
            continue
        eligible.append(func)
        score, detail = score_function(arch, func, insns)
        scored.append((score, func.name, func, detail))
    rng = random.Random(seed + {"x86": 17, "amd64": 31, "arm": 43, "arm64": 47}[arch])
    random_sample = list(eligible) if all_functions else rng.sample(eligible, min(random_count, len(eligible)))
    scored.sort(key=lambda item: (-item[0], item[1]))
    target_sample = [] if all_functions else [item[2] for item in scored[:min(target_count, len(scored))]]
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
        "detail_resolutions": 0,
        "instructions": 0,
    }
    for result in results:
        crit = len(result.get("critical_mismatches", []))
        if crit:
            summary["functions_with_critical_mismatches"] += 1
            summary["critical_mismatches"] += crit
        summary["alias_differences"] += len(result.get("alias_differences", []))
        summary["detail_resolutions"] += result.get("detail_resolution_count", len(result.get("detail_resolutions", [])))
        summary["instructions"] += result.get("instruction_count", 0)
    return summary


def import_capstone(pydeps: Path):
    if pydeps.exists():
        sys.path.insert(0, str(pydeps))
    import capstone
    return capstone


@dataclass(frozen=True)
class ImageTask:
    arch: str
    kind: str
    path: Path
    image: object


def pe_arch_from_machine(machine: int):
    return {
        0x014c: "x86",
        0x8664: "amd64",
        0x01c0: "arm",
        0x01c2: "arm",
        0x01c4: "arm",
        0xaa64: "arm64",
    }.get(machine)


def elf_arch_from_machine(machine: int):
    return {
        3: "x86",
        62: "amd64",
        40: "arm",
        183: "arm64",
    }.get(machine)


def load_pe_task(path: Path):
    image = PEImage(path)
    arch = pe_arch_from_machine(image.machine)
    if arch is None:
        return None
    return ImageTask(arch, "pe", path, image)


def load_elf_task(path: Path):
    image = ELFImage(path)
    arch = elf_arch_from_machine(image.machine)
    if arch is None:
        return None
    return ImageTask(arch, "elf", path, image)


def discover_pe_tasks(windows_kernel_dir: Path) -> list[ImageTask]:
    tasks = []
    for path in sorted(windows_kernel_dir.rglob("*.exe")):
        try:
            task = load_pe_task(path)
        except ValueError:
            continue
        if task is not None:
            tasks.append(task)
    return tasks


def discover_elf_tasks(elf_corpus_dir: Path) -> list[ImageTask]:
    tasks = []
    if not elf_corpus_dir.exists():
        return tasks
    for path in sorted(p for p in elf_corpus_dir.rglob("*") if p.is_file()):
        try:
            task = load_elf_task(path)
        except (OSError, ValueError, struct.error):
            continue
        if task is not None:
            tasks.append(task)
    return tasks


def task_display_path(root: Path, path: Path) -> str:
    try:
        return str(path.relative_to(root))
    except ValueError:
        return str(path)


def main() -> int:
    parser = argparse.ArgumentParser(description="Run real-world AsmKit decode cross-checks against Capstone and llvm-mc.")
    parser.add_argument("--root", type=Path, default=Path.cwd())
    parser.add_argument("--windows-kernel-dir", type=Path, default=Path("ntoskrnl"))
    parser.add_argument("--elf-corpus-dir", type=Path, default=Path("oth/isa_corpus_deb/binaries"))
    parser.add_argument("--helper", type=Path, default=Path("build-asmkit-runtime-text/Debug/asmkit_decode_dump.exe"))
    parser.add_argument("--llvm-mc", type=Path, default=Path(r"E:\cross-llvm\clang+llvm-22.1.2-x86_64-pc-windows-msvc\bin\llvm-mc.exe"))
    parser.add_argument("--pydeps", type=Path, default=Path("build-asmkit-runtime-text/pydeps"))
    parser.add_argument("--report", type=Path, default=Path("build-asmkit-runtime-text/real_world_test_report.json"))
    parser.add_argument("--max-window", type=int, default=4096)
    parser.add_argument("--max-instructions", type=int, default=96)
    parser.add_argument("--random-count", type=int, default=100)
    parser.add_argument("--target-count", type=int, default=50)
    parser.add_argument("--sample", action="store_true", help="compare a random/targeted subset instead of every recognizable function")
    parser.add_argument("--arch", action="append", choices=("x86", "amd64", "arm", "arm64"), help="limit to one architecture; may be repeated")
    parser.add_argument("--path-contains", action="append", default=[], help="limit images to paths containing this case-insensitive substring; may be repeated")
    parser.add_argument("--seed", type=lambda s: int(s, 0), default=0xA5C0DE)
    parser.add_argument("--max-detail-resolutions-per-function", type=int, default=128, help="0 keeps every resolved detail diagnostic in the JSON report")
    args = parser.parse_args()

    root = args.root.resolve()
    windows_kernel_dir = (root / args.windows_kernel_dir).resolve()
    elf_corpus_dir = (root / args.elf_corpus_dir).resolve()
    helper = (root / args.helper).resolve()
    llvm_mc = args.llvm_mc if args.llvm_mc.is_absolute() else (root / args.llvm_mc)
    llvm_mc = llvm_mc.resolve()
    pydeps = (root / args.pydeps).resolve()
    report = (root / args.report).resolve()
    if not helper.exists():
        raise SystemExit(f"helper not found: {helper}")
    if not llvm_mc.exists():
        raise SystemExit(f"llvm-mc not found: {llvm_mc}")
    capstone = import_capstone(pydeps)
    tasks = []
    if windows_kernel_dir.exists():
        tasks.extend(discover_pe_tasks(windows_kernel_dir))
    if elf_corpus_dir.exists():
        tasks.extend(discover_elf_tasks(elf_corpus_dir))
    if args.arch:
        wanted_arches = set(args.arch)
        tasks = [task for task in tasks if task.arch in wanted_arches]
    if args.path_contains:
        needles = [item.lower() for item in args.path_contains]
        tasks = [task for task in tasks if any(needle in str(task.path).lower() for needle in needles)]
    if not tasks:
        raise SystemExit("no PE/ELF images found for real-world testing")
    tasks.sort(key=lambda item: (item.arch, item.kind, str(item.path).lower()))
    all_functions = not args.sample
    final = {
        "capstone_version": getattr(capstone, "__version__", "unknown"),
        "llvm_mc": str(llvm_mc),
        "max_window": args.max_window,
        "max_instructions": args.max_instructions,
        "random_count_requested": args.random_count,
        "target_count_requested": args.target_count,
        "all_functions": all_functions,
        "seed": args.seed,
        "corpora": {
            "windows_kernel_dir": str(windows_kernel_dir),
            "elf_corpus_dir": str(elf_corpus_dir),
        },
        "architectures": {},
    }
    overall_critical = 0
    for arch in ("x86", "amd64", "arm", "arm64"):
        arch_tasks = [task for task in tasks if task.arch == arch]
        if not arch_tasks:
            continue
        arch_results = []
        images = []
        md_cache = {}
        for task in arch_tasks:
            image = task.image
            candidates = image.exported_functions(args.max_window, thumb_entries=(task.kind == "pe" and arch == "arm"))
            if not candidates:
                images.append({
                    "path": task_display_path(root, task.path),
                    "kind": task.kind,
                    "machine": getattr(image, "machine", 0),
                    "export_executable_candidates": 0,
                    "eligible_with_capstone_instructions": 0,
                    "summary": summarize_results([]),
                    "results": [],
                })
                continue
            print(f"{arch}: image={task_display_path(root, task.path)} kind={task.kind} candidates={len(candidates)}")
            random_sample, target_sample, union_sample, score_by_name, eligible_count = select_samples(
                arch,
                image,
                candidates,
                md_cache,
                capstone,
                args.random_count,
                args.target_count,
                args.seed,
                args.max_instructions,
                all_functions,
            )
            if args.sample and len(random_sample) < args.random_count:
                raise SystemExit(f"{arch} {task.path}: only {len(random_sample)} random-eligible functions")
            if args.sample and len(target_sample) < args.target_count:
                raise SystemExit(f"{arch} {task.path}: only {len(target_sample)} target-eligible functions")
            print(f"{arch}: eligible={eligible_count} selected={len(union_sample)}")
            results = []
            for index, func in enumerate(union_sample, 1):
                md = capstone_decoder(md_cache, capstone, arch, func.mode)
                result = compare_function(
                    helper,
                    llvm_mc,
                    arch,
                    image,
                    md,
                    func,
                    args.max_instructions,
                    args.max_detail_resolutions_per_function,
                )
                results.append(result)
                if result["critical_mismatches"]:
                    first = result["critical_mismatches"][0]
                    print(f"{arch}: mismatch {task_display_path(root, task.path)}::{func.name} +0x{first.get('offset', 0):x} {first.get('kind')}")
                elif index % 50 == 0:
                    print(f"{arch}: compared {index}/{len(union_sample)} in {task_display_path(root, task.path)}")
            summary = summarize_results(results)
            overall_critical += summary["critical_mismatches"]
            arch_results.extend(results)
            images.append({
                "path": task_display_path(root, task.path),
                "kind": task.kind,
                "machine": getattr(image, "machine", 0),
                "image_base": getattr(image, "image_base", 0),
                "export_executable_candidates": len(candidates),
                "eligible_with_capstone_instructions": eligible_count,
                "random_functions": [func.name for func in random_sample],
                "targeted_functions": [
                    {
                        "name": func.name,
                        "export_rva": func.export_rva,
                        "rva": func.rva,
                        "mode": func.mode,
                        "score": score_by_name.get(func.name, {}).get("score", 0),
                        "reasons": score_by_name.get(func.name, {}).get("reasons", []),
                        "rare_hits": score_by_name.get(func.name, {}).get("rare_hits", {}),
                    }
                    for func in target_sample
                ],
                "unique_functions_compared": [func.name for func in union_sample],
                "summary": summary,
                "results": results,
            })
            print(f"{arch}: image summary {summary}")
        final["architectures"][arch] = {
            "summary": summarize_results(arch_results),
            "images": images,
        }
    report.parent.mkdir(parents=True, exist_ok=True)
    report.write_text(json.dumps(final, indent=2), encoding="utf-8")
    print(f"report={report}")
    print(f"overall_critical_mismatches={overall_critical}")
    return 1 if overall_critical else 0


if __name__ == "__main__":
    raise SystemExit(main())
