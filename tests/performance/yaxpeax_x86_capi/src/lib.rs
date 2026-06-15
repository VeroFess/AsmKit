use core::ffi::c_void;

use yaxpeax_arch::LengthedInstruction;

const YAXPEAX_MAX_OPERANDS: usize = 8;
const YAXPEAX_MAX_INST_BYTES: usize = 16;

const YAXPEAX_OPERAND_NONE: u8 = 0;
const YAXPEAX_OPERAND_REG: u8 = 1;
const YAXPEAX_OPERAND_IMM: u8 = 2;
const YAXPEAX_OPERAND_MEM: u8 = 3;
const YAXPEAX_OPERAND_FAR: u8 = 4;

const YAXPEAX_OPERAND_FLAG_SIGNED: u32 = 1u32 << 0;
const YAXPEAX_OPERAND_FLAG_MEMORY: u32 = 1u32 << 1;
const YAXPEAX_OPERAND_FLAG_MASKED: u32 = 1u32 << 2;
const YAXPEAX_OPERAND_FLAG_ZEROING: u32 = 1u32 << 3;
const YAXPEAX_OPERAND_FLAG_SAE: u32 = 1u32 << 4;

#[repr(C)]
#[derive(Copy, Clone)]
pub struct YaxpeaxOperandResult {
    pub imm: i64,
    pub disp: i64,
    pub address: u64,
    pub flags: u32,
    pub width: u16,
    pub reg: u16,
    pub base: u16,
    pub index: u16,
    pub mask: u16,
    pub kind: u8,
    pub scale: u8,
    pub decorator: u8,
    pub reserved: u8,
}

const YAXPEAX_EMPTY_OPERAND: YaxpeaxOperandResult = YaxpeaxOperandResult {
    imm: 0,
    disp: 0,
    address: 0,
    flags: 0,
    width: 0,
    reg: 0,
    base: 0,
    index: 0,
    mask: 0,
    kind: YAXPEAX_OPERAND_NONE,
    scale: 0,
    decorator: 0,
    reserved: 0,
};

#[repr(C)]
pub struct YaxpeaxDecodeResult {
    pub operands: [YaxpeaxOperandResult; YAXPEAX_MAX_OPERANDS],
    pub bytes: [u8; YAXPEAX_MAX_INST_BYTES],
    pub opcode: u32,
    pub len: u8,
    pub operand_count: u8,
    pub mem_width: u16,
}

struct YaxpeaxX86Context {
    long_decoder: yaxpeax_x86::long_mode::InstDecoder,
    protected_decoder: yaxpeax_x86::protected_mode::InstDecoder,
}

#[no_mangle]
pub extern "C" fn asmkit_yaxpeax_x86_create() -> *mut c_void {
    let context = Box::new(YaxpeaxX86Context {
        long_decoder: yaxpeax_x86::long_mode::InstDecoder::default(),
        protected_decoder: yaxpeax_x86::protected_mode::InstDecoder::default(),
    });
    Box::into_raw(context) as *mut c_void
}

#[no_mangle]
pub unsafe extern "C" fn asmkit_yaxpeax_x86_destroy(context: *mut c_void) {
    if !context.is_null() {
        drop(Box::from_raw(context as *mut YaxpeaxX86Context));
    }
}

fn long_reg_class_id(reg: yaxpeax_x86::long_mode::RegSpec) -> u8 {
    use yaxpeax_x86::long_mode::register_class as class;
    let reg_class = reg.class();
    if reg_class == class::Q {
        1
    } else if reg_class == class::D {
        2
    } else if reg_class == class::W {
        3
    } else if reg_class == class::B {
        4
    } else if reg_class == class::RB {
        5
    } else if reg_class == class::CR {
        6
    } else if reg_class == class::DR {
        7
    } else if reg_class == class::S {
        8
    } else if reg_class == class::X {
        9
    } else if reg_class == class::Y {
        10
    } else if reg_class == class::Z {
        11
    } else if reg_class == class::ST {
        12
    } else if reg_class == class::MM {
        13
    } else if reg_class == class::K {
        14
    } else if reg_class == class::EIP {
        15
    } else if reg_class == class::RIP {
        16
    } else if reg_class == class::EFLAGS {
        17
    } else if reg_class == class::RFLAGS {
        18
    } else {
        0
    }
}

fn protected_reg_class_id(reg: yaxpeax_x86::protected_mode::RegSpec) -> u8 {
    use yaxpeax_x86::protected_mode::register_class as class;
    let reg_class = reg.class();
    if reg_class == class::D {
        2
    } else if reg_class == class::W {
        3
    } else if reg_class == class::B {
        4
    } else if reg_class == class::CR {
        6
    } else if reg_class == class::DR {
        7
    } else if reg_class == class::S {
        8
    } else if reg_class == class::X {
        9
    } else if reg_class == class::Y {
        10
    } else if reg_class == class::Z {
        11
    } else if reg_class == class::ST {
        12
    } else if reg_class == class::MM {
        13
    } else if reg_class == class::K {
        14
    } else if reg_class == class::EIP {
        15
    } else if reg_class == class::EFLAGS {
        17
    } else {
        0
    }
}

fn long_reg_id(reg: yaxpeax_x86::long_mode::RegSpec) -> u16 {
    ((long_reg_class_id(reg) as u16) << 8) | reg.num() as u16
}

fn protected_reg_id(reg: yaxpeax_x86::protected_mode::RegSpec) -> u16 {
    ((protected_reg_class_id(reg) as u16) << 8) | reg.num() as u16
}

fn long_mem_width(inst: &yaxpeax_x86::long_mode::Instruction) -> u16 {
    inst.mem_size()
        .and_then(|size| size.bytes_size())
        .map(|bytes| bytes as u16 * 8u16)
        .unwrap_or(0)
}

fn protected_mem_width(inst: &yaxpeax_x86::protected_mode::Instruction) -> u16 {
    inst.mem_size()
        .and_then(|size| size.bytes_size())
        .map(|bytes| bytes as u16 * 8u16)
        .unwrap_or(0)
}

fn long_width_or_mem(
    inst: &yaxpeax_x86::long_mode::Instruction,
    operand: &yaxpeax_x86::long_mode::Operand,
) -> u16 {
    operand.width().map(|bytes| bytes as u16 * 8u16).unwrap_or_else(|| long_mem_width(inst))
}

fn protected_width_or_mem(
    inst: &yaxpeax_x86::protected_mode::Instruction,
    operand: &yaxpeax_x86::protected_mode::Operand,
) -> u16 {
    operand.width().map(|bytes| bytes as u16 * 8u16).unwrap_or_else(|| protected_mem_width(inst))
}

fn long_merge_flags(merge: yaxpeax_x86::long_mode::MergeMode) -> u32 {
    match merge {
        yaxpeax_x86::long_mode::MergeMode::Zero => YAXPEAX_OPERAND_FLAG_ZEROING,
        _ => 0,
    }
}

fn protected_merge_flags(merge: yaxpeax_x86::protected_mode::MergeMode) -> u32 {
    match merge {
        yaxpeax_x86::protected_mode::MergeMode::Zero => YAXPEAX_OPERAND_FLAG_ZEROING,
        _ => 0,
    }
}

fn long_sae_code(sae: yaxpeax_x86::long_mode::SaeMode) -> u8 {
    match sae {
        yaxpeax_x86::long_mode::SaeMode::RoundNearest => 1,
        yaxpeax_x86::long_mode::SaeMode::RoundDown => 2,
        yaxpeax_x86::long_mode::SaeMode::RoundUp => 3,
        yaxpeax_x86::long_mode::SaeMode::RoundZero => 4,
    }
}

fn protected_sae_code(sae: yaxpeax_x86::protected_mode::SaeMode) -> u8 {
    match sae {
        yaxpeax_x86::protected_mode::SaeMode::RoundNearest => 1,
        yaxpeax_x86::protected_mode::SaeMode::RoundDown => 2,
        yaxpeax_x86::protected_mode::SaeMode::RoundUp => 3,
        yaxpeax_x86::protected_mode::SaeMode::RoundZero => 4,
    }
}

fn long_operand_result(
    inst: &yaxpeax_x86::long_mode::Instruction,
    operand: yaxpeax_x86::long_mode::Operand,
) -> YaxpeaxOperandResult {
    use yaxpeax_x86::long_mode::Operand;
    let mut out = YAXPEAX_EMPTY_OPERAND;
    out.width = long_width_or_mem(inst, &operand);
    match operand {
        Operand::ImmediateI8 { imm } => {
            out.kind = YAXPEAX_OPERAND_IMM;
            out.flags = YAXPEAX_OPERAND_FLAG_SIGNED;
            out.imm = imm as i64;
        }
        Operand::ImmediateI16 { imm } => {
            out.kind = YAXPEAX_OPERAND_IMM;
            out.flags = YAXPEAX_OPERAND_FLAG_SIGNED;
            out.imm = imm as i64;
        }
        Operand::ImmediateI32 { imm } => {
            out.kind = YAXPEAX_OPERAND_IMM;
            out.flags = YAXPEAX_OPERAND_FLAG_SIGNED;
            out.imm = imm as i64;
        }
        Operand::ImmediateI64 { imm } => {
            out.kind = YAXPEAX_OPERAND_IMM;
            out.flags = YAXPEAX_OPERAND_FLAG_SIGNED;
            out.imm = imm;
        }
        Operand::ImmediateU8 { imm } => {
            out.kind = YAXPEAX_OPERAND_IMM;
            out.imm = imm as i64;
        }
        Operand::ImmediateU16 { imm } => {
            out.kind = YAXPEAX_OPERAND_IMM;
            out.imm = imm as i64;
        }
        Operand::ImmediateU32 { imm } => {
            out.kind = YAXPEAX_OPERAND_IMM;
            out.imm = imm as i64;
        }
        Operand::ImmediateU64 { imm } => {
            out.kind = YAXPEAX_OPERAND_IMM;
            out.imm = imm as i64;
        }
        Operand::Register { reg } => {
            out.kind = YAXPEAX_OPERAND_REG;
            out.reg = long_reg_id(reg);
            out.width = reg.width() as u16 * 8u16;
        }
        Operand::RegisterMaskMerge { reg, mask, merge } => {
            out.kind = YAXPEAX_OPERAND_REG;
            out.flags = YAXPEAX_OPERAND_FLAG_MASKED | long_merge_flags(merge);
            out.reg = long_reg_id(reg);
            out.mask = long_reg_id(mask);
            out.width = reg.width() as u16 * 8u16;
        }
        Operand::RegisterMaskMergeSae { reg, mask, merge, sae } => {
            out.kind = YAXPEAX_OPERAND_REG;
            out.flags = YAXPEAX_OPERAND_FLAG_MASKED | YAXPEAX_OPERAND_FLAG_SAE | long_merge_flags(merge);
            out.reg = long_reg_id(reg);
            out.mask = long_reg_id(mask);
            out.width = reg.width() as u16 * 8u16;
            out.decorator = long_sae_code(sae);
        }
        Operand::RegisterMaskMergeSaeNoround { reg, mask, merge } => {
            out.kind = YAXPEAX_OPERAND_REG;
            out.flags = YAXPEAX_OPERAND_FLAG_MASKED | YAXPEAX_OPERAND_FLAG_SAE | long_merge_flags(merge);
            out.reg = long_reg_id(reg);
            out.mask = long_reg_id(mask);
            out.width = reg.width() as u16 * 8u16;
        }
        Operand::AbsoluteU32 { addr } => {
            out.kind = YAXPEAX_OPERAND_MEM;
            out.flags = YAXPEAX_OPERAND_FLAG_MEMORY;
            out.address = addr as u64;
        }
        Operand::AbsoluteU64 { addr } => {
            out.kind = YAXPEAX_OPERAND_MEM;
            out.flags = YAXPEAX_OPERAND_FLAG_MEMORY;
            out.address = addr;
        }
        Operand::MemDeref { base } => {
            out.kind = YAXPEAX_OPERAND_MEM;
            out.flags = YAXPEAX_OPERAND_FLAG_MEMORY;
            out.base = long_reg_id(base);
        }
        Operand::Disp { base, disp } => {
            out.kind = YAXPEAX_OPERAND_MEM;
            out.flags = YAXPEAX_OPERAND_FLAG_MEMORY;
            out.base = long_reg_id(base);
            out.disp = disp as i64;
        }
        Operand::MemIndexScale { index, scale } => {
            out.kind = YAXPEAX_OPERAND_MEM;
            out.flags = YAXPEAX_OPERAND_FLAG_MEMORY;
            out.index = long_reg_id(index);
            out.scale = scale;
        }
        Operand::MemIndexScaleDisp { index, scale, disp } => {
            out.kind = YAXPEAX_OPERAND_MEM;
            out.flags = YAXPEAX_OPERAND_FLAG_MEMORY;
            out.index = long_reg_id(index);
            out.scale = scale;
            out.disp = disp as i64;
        }
        Operand::MemBaseIndexScale { base, index, scale } => {
            out.kind = YAXPEAX_OPERAND_MEM;
            out.flags = YAXPEAX_OPERAND_FLAG_MEMORY;
            out.base = long_reg_id(base);
            out.index = long_reg_id(index);
            out.scale = scale;
        }
        Operand::MemBaseIndexScaleDisp { base, index, scale, disp } => {
            out.kind = YAXPEAX_OPERAND_MEM;
            out.flags = YAXPEAX_OPERAND_FLAG_MEMORY;
            out.base = long_reg_id(base);
            out.index = long_reg_id(index);
            out.scale = scale;
            out.disp = disp as i64;
        }
        Operand::MemDerefMasked { base, mask } => {
            out.kind = YAXPEAX_OPERAND_MEM;
            out.flags = YAXPEAX_OPERAND_FLAG_MEMORY | YAXPEAX_OPERAND_FLAG_MASKED;
            out.base = long_reg_id(base);
            out.mask = long_reg_id(mask);
        }
        Operand::DispMasked { base, disp, mask } => {
            out.kind = YAXPEAX_OPERAND_MEM;
            out.flags = YAXPEAX_OPERAND_FLAG_MEMORY | YAXPEAX_OPERAND_FLAG_MASKED;
            out.base = long_reg_id(base);
            out.mask = long_reg_id(mask);
            out.disp = disp as i64;
        }
        Operand::MemIndexScaleMasked { index, scale, mask } => {
            out.kind = YAXPEAX_OPERAND_MEM;
            out.flags = YAXPEAX_OPERAND_FLAG_MEMORY | YAXPEAX_OPERAND_FLAG_MASKED;
            out.index = long_reg_id(index);
            out.mask = long_reg_id(mask);
            out.scale = scale;
        }
        Operand::MemIndexScaleDispMasked { index, scale, disp, mask } => {
            out.kind = YAXPEAX_OPERAND_MEM;
            out.flags = YAXPEAX_OPERAND_FLAG_MEMORY | YAXPEAX_OPERAND_FLAG_MASKED;
            out.index = long_reg_id(index);
            out.mask = long_reg_id(mask);
            out.scale = scale;
            out.disp = disp as i64;
        }
        Operand::MemBaseIndexScaleMasked { base, index, scale, mask } => {
            out.kind = YAXPEAX_OPERAND_MEM;
            out.flags = YAXPEAX_OPERAND_FLAG_MEMORY | YAXPEAX_OPERAND_FLAG_MASKED;
            out.base = long_reg_id(base);
            out.index = long_reg_id(index);
            out.mask = long_reg_id(mask);
            out.scale = scale;
        }
        Operand::MemBaseIndexScaleDispMasked { base, index, scale, disp, mask } => {
            out.kind = YAXPEAX_OPERAND_MEM;
            out.flags = YAXPEAX_OPERAND_FLAG_MEMORY | YAXPEAX_OPERAND_FLAG_MASKED;
            out.base = long_reg_id(base);
            out.index = long_reg_id(index);
            out.mask = long_reg_id(mask);
            out.scale = scale;
            out.disp = disp as i64;
        }
        Operand::Nothing => {}
        _ => {}
    }
    out
}

fn protected_operand_result(
    inst: &yaxpeax_x86::protected_mode::Instruction,
    operand: yaxpeax_x86::protected_mode::Operand,
) -> YaxpeaxOperandResult {
    use yaxpeax_x86::protected_mode::Operand;
    let mut out = YAXPEAX_EMPTY_OPERAND;
    out.width = protected_width_or_mem(inst, &operand);
    match operand {
        Operand::ImmediateI8 { imm } => {
            out.kind = YAXPEAX_OPERAND_IMM;
            out.flags = YAXPEAX_OPERAND_FLAG_SIGNED;
            out.imm = imm as i64;
        }
        Operand::ImmediateI16 { imm } => {
            out.kind = YAXPEAX_OPERAND_IMM;
            out.flags = YAXPEAX_OPERAND_FLAG_SIGNED;
            out.imm = imm as i64;
        }
        Operand::ImmediateI32 { imm } => {
            out.kind = YAXPEAX_OPERAND_IMM;
            out.flags = YAXPEAX_OPERAND_FLAG_SIGNED;
            out.imm = imm as i64;
        }
        Operand::ImmediateU8 { imm } => {
            out.kind = YAXPEAX_OPERAND_IMM;
            out.imm = imm as i64;
        }
        Operand::ImmediateU16 { imm } => {
            out.kind = YAXPEAX_OPERAND_IMM;
            out.imm = imm as i64;
        }
        Operand::ImmediateU32 { imm } => {
            out.kind = YAXPEAX_OPERAND_IMM;
            out.imm = imm as i64;
        }
        Operand::Register { reg } => {
            out.kind = YAXPEAX_OPERAND_REG;
            out.reg = protected_reg_id(reg);
            out.width = reg.width() as u16 * 8u16;
        }
        Operand::RegisterMaskMerge { reg, mask, merge } => {
            out.kind = YAXPEAX_OPERAND_REG;
            out.flags = YAXPEAX_OPERAND_FLAG_MASKED | protected_merge_flags(merge);
            out.reg = protected_reg_id(reg);
            out.mask = protected_reg_id(mask);
            out.width = reg.width() as u16 * 8u16;
        }
        Operand::RegisterMaskMergeSae { reg, mask, merge, sae } => {
            out.kind = YAXPEAX_OPERAND_REG;
            out.flags = YAXPEAX_OPERAND_FLAG_MASKED | YAXPEAX_OPERAND_FLAG_SAE | protected_merge_flags(merge);
            out.reg = protected_reg_id(reg);
            out.mask = protected_reg_id(mask);
            out.width = reg.width() as u16 * 8u16;
            out.decorator = protected_sae_code(sae);
        }
        Operand::RegisterMaskMergeSaeNoround { reg, mask, merge } => {
            out.kind = YAXPEAX_OPERAND_REG;
            out.flags = YAXPEAX_OPERAND_FLAG_MASKED | YAXPEAX_OPERAND_FLAG_SAE | protected_merge_flags(merge);
            out.reg = protected_reg_id(reg);
            out.mask = protected_reg_id(mask);
            out.width = reg.width() as u16 * 8u16;
        }
        Operand::AbsoluteU16 { addr } => {
            out.kind = YAXPEAX_OPERAND_MEM;
            out.flags = YAXPEAX_OPERAND_FLAG_MEMORY;
            out.address = addr as u64;
        }
        Operand::AbsoluteU32 { addr } => {
            out.kind = YAXPEAX_OPERAND_MEM;
            out.flags = YAXPEAX_OPERAND_FLAG_MEMORY;
            out.address = addr as u64;
        }
        Operand::MemDeref { base } => {
            out.kind = YAXPEAX_OPERAND_MEM;
            out.flags = YAXPEAX_OPERAND_FLAG_MEMORY;
            out.base = protected_reg_id(base);
        }
        Operand::Disp { base, disp } => {
            out.kind = YAXPEAX_OPERAND_MEM;
            out.flags = YAXPEAX_OPERAND_FLAG_MEMORY;
            out.base = protected_reg_id(base);
            out.disp = disp as i64;
        }
        Operand::MemIndexScale { index, scale } => {
            out.kind = YAXPEAX_OPERAND_MEM;
            out.flags = YAXPEAX_OPERAND_FLAG_MEMORY;
            out.index = protected_reg_id(index);
            out.scale = scale;
        }
        Operand::MemIndexScaleDisp { index, scale, disp } => {
            out.kind = YAXPEAX_OPERAND_MEM;
            out.flags = YAXPEAX_OPERAND_FLAG_MEMORY;
            out.index = protected_reg_id(index);
            out.scale = scale;
            out.disp = disp as i64;
        }
        Operand::MemBaseIndexScale { base, index, scale } => {
            out.kind = YAXPEAX_OPERAND_MEM;
            out.flags = YAXPEAX_OPERAND_FLAG_MEMORY;
            out.base = protected_reg_id(base);
            out.index = protected_reg_id(index);
            out.scale = scale;
        }
        Operand::MemBaseIndexScaleDisp { base, index, scale, disp } => {
            out.kind = YAXPEAX_OPERAND_MEM;
            out.flags = YAXPEAX_OPERAND_FLAG_MEMORY;
            out.base = protected_reg_id(base);
            out.index = protected_reg_id(index);
            out.scale = scale;
            out.disp = disp as i64;
        }
        Operand::MemDerefMasked { base, mask } => {
            out.kind = YAXPEAX_OPERAND_MEM;
            out.flags = YAXPEAX_OPERAND_FLAG_MEMORY | YAXPEAX_OPERAND_FLAG_MASKED;
            out.base = protected_reg_id(base);
            out.mask = protected_reg_id(mask);
        }
        Operand::DispMasked { base, disp, mask } => {
            out.kind = YAXPEAX_OPERAND_MEM;
            out.flags = YAXPEAX_OPERAND_FLAG_MEMORY | YAXPEAX_OPERAND_FLAG_MASKED;
            out.base = protected_reg_id(base);
            out.mask = protected_reg_id(mask);
            out.disp = disp as i64;
        }
        Operand::MemIndexScaleMasked { index, scale, mask } => {
            out.kind = YAXPEAX_OPERAND_MEM;
            out.flags = YAXPEAX_OPERAND_FLAG_MEMORY | YAXPEAX_OPERAND_FLAG_MASKED;
            out.index = protected_reg_id(index);
            out.mask = protected_reg_id(mask);
            out.scale = scale;
        }
        Operand::MemIndexScaleDispMasked { index, scale, disp, mask } => {
            out.kind = YAXPEAX_OPERAND_MEM;
            out.flags = YAXPEAX_OPERAND_FLAG_MEMORY | YAXPEAX_OPERAND_FLAG_MASKED;
            out.index = protected_reg_id(index);
            out.mask = protected_reg_id(mask);
            out.scale = scale;
            out.disp = disp as i64;
        }
        Operand::MemBaseIndexScaleMasked { base, index, scale, mask } => {
            out.kind = YAXPEAX_OPERAND_MEM;
            out.flags = YAXPEAX_OPERAND_FLAG_MEMORY | YAXPEAX_OPERAND_FLAG_MASKED;
            out.base = protected_reg_id(base);
            out.index = protected_reg_id(index);
            out.mask = protected_reg_id(mask);
            out.scale = scale;
        }
        Operand::MemBaseIndexScaleDispMasked { base, index, scale, disp, mask } => {
            out.kind = YAXPEAX_OPERAND_MEM;
            out.flags = YAXPEAX_OPERAND_FLAG_MEMORY | YAXPEAX_OPERAND_FLAG_MASKED;
            out.base = protected_reg_id(base);
            out.index = protected_reg_id(index);
            out.mask = protected_reg_id(mask);
            out.scale = scale;
            out.disp = disp as i64;
        }
        Operand::AbsoluteFarAddress { segment, address } => {
            out.kind = YAXPEAX_OPERAND_FAR;
            out.imm = segment as i64;
            out.address = address as u64;
            out.width = 48;
        }
        Operand::Nothing => {}
        _ => {}
    }
    out
}

#[no_mangle]
pub unsafe extern "C" fn asmkit_yaxpeax_x86_64_decode_one(
    context: *const c_void,
    data: *const u8,
    len: usize,
    out_result: *mut YaxpeaxDecodeResult,
) -> i32 {
    if context.is_null() || data.is_null() || out_result.is_null() {
        return 0;
    }
    let context = &*(context as *const YaxpeaxX86Context);
    let bytes = core::slice::from_raw_parts(data, len);
    match context.long_decoder.decode_slice(bytes) {
        Ok(inst) => {
            let mut result = YaxpeaxDecodeResult {
                operands: [YAXPEAX_EMPTY_OPERAND; YAXPEAX_MAX_OPERANDS],
                bytes: [0; YAXPEAX_MAX_INST_BYTES],
                opcode: inst.opcode() as u32,
                len: inst.len().to_const() as u8,
                operand_count: inst.operand_count(),
                mem_width: long_mem_width(&inst),
            };
            let byte_count = core::cmp::min(core::cmp::min(result.len as usize, bytes.len()), YAXPEAX_MAX_INST_BYTES);
            result.bytes[..byte_count].copy_from_slice(&bytes[..byte_count]);
            let limit = core::cmp::min(result.operand_count as usize, 4usize);
            for i in 0..limit {
                if inst.operand_present(i as u8) {
                    result.operands[i] = long_operand_result(&inst, inst.operand(i as u8));
                }
            }
            *out_result = result;
            1
        }
        Err(_) => 0,
    }
}

#[no_mangle]
pub unsafe extern "C" fn asmkit_yaxpeax_x86_32_decode_one(
    context: *const c_void,
    data: *const u8,
    len: usize,
    out_result: *mut YaxpeaxDecodeResult,
) -> i32 {
    if context.is_null() || data.is_null() || out_result.is_null() {
        return 0;
    }
    let context = &*(context as *const YaxpeaxX86Context);
    let bytes = core::slice::from_raw_parts(data, len);
    match context.protected_decoder.decode_slice(bytes) {
        Ok(inst) => {
            let mut result = YaxpeaxDecodeResult {
                operands: [YAXPEAX_EMPTY_OPERAND; YAXPEAX_MAX_OPERANDS],
                bytes: [0; YAXPEAX_MAX_INST_BYTES],
                opcode: inst.opcode() as u32,
                len: inst.len().to_const() as u8,
                operand_count: inst.operand_count(),
                mem_width: protected_mem_width(&inst),
            };
            let byte_count = core::cmp::min(core::cmp::min(result.len as usize, bytes.len()), YAXPEAX_MAX_INST_BYTES);
            result.bytes[..byte_count].copy_from_slice(&bytes[..byte_count]);
            let limit = core::cmp::min(result.operand_count as usize, 4usize);
            for i in 0..limit {
                if inst.operand_present(i as u8) {
                    result.operands[i] = protected_operand_result(&inst, inst.operand(i as u8));
                }
            }
            *out_result = result;
            1
        }
        Err(_) => 0,
    }
}
