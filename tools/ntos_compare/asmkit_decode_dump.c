#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "asmkit/asmkit_easy.h"

static int hex_digit(int ch)
{
    if (ch >= '0' && ch <= '9') {
        return ch - '0';
    }
    ch = tolower((unsigned char)ch);
    if (ch >= 'a' && ch <= 'f') {
        return ch - 'a' + 10;
    }
    return -1;
}

static size_t parse_hex(const char* text, uint8_t* out, size_t cap)
{
    size_t count = 0u;
    int high = -1;
    while (*text != '\0') {
        int d = hex_digit((unsigned char)*text++);
        if (d < 0) {
            continue;
        }
        if (high < 0) {
            high = d;
        } else {
            if (count >= cap) {
                return 0u;
            }
            out[count++] = (uint8_t)((high << 4) | d);
            high = -1;
        }
    }
    return high < 0 ? count : 0u;
}

static const char* status_name(asmkit_status_t status)
{
    switch (status) {
    case ASMKIT_OK: return "ASMKIT_OK";
    case ASMKIT_ERR_INVALID_ARGUMENT: return "ASMKIT_ERR_INVALID_ARGUMENT";
    case ASMKIT_ERR_OUTPUT_TOO_SMALL: return "ASMKIT_ERR_OUTPUT_TOO_SMALL";
    case ASMKIT_ERR_UNSUPPORTED_ARCH: return "ASMKIT_ERR_UNSUPPORTED_ARCH";
    case ASMKIT_ERR_UNSUPPORTED_MODE: return "ASMKIT_ERR_UNSUPPORTED_MODE";
    case ASMKIT_ERR_UNSUPPORTED_INSTRUCTION: return "ASMKIT_ERR_UNSUPPORTED_INSTRUCTION";
    case ASMKIT_ERR_UNSUPPORTED_FEATURE: return "ASMKIT_ERR_UNSUPPORTED_FEATURE";
    case ASMKIT_ERR_UNSUPPORTED_TARGET_OPERATION: return "ASMKIT_ERR_UNSUPPORTED_TARGET_OPERATION";
    case ASMKIT_ERR_DECODE_FAILED: return "ASMKIT_ERR_DECODE_FAILED";
    case ASMKIT_ERR_ENCODE_FAILED: return "ASMKIT_ERR_ENCODE_FAILED";
    case ASMKIT_ERR_WORKSPACE_TOO_SMALL: return "ASMKIT_ERR_WORKSPACE_TOO_SMALL";
    case ASMKIT_ERR_UNSUPPORTED_RELOCATION: return "ASMKIT_ERR_UNSUPPORTED_RELOCATION";
    case ASMKIT_ERR_BRANCH_OUT_OF_RANGE: return "ASMKIT_ERR_BRANCH_OUT_OF_RANGE";
    case ASMKIT_ERR_REQUIRES_BRANCH_ISLAND: return "ASMKIT_ERR_REQUIRES_BRANCH_ISLAND";
    case ASMKIT_ERR_FORBIDDEN_CLOBBER: return "ASMKIT_ERR_FORBIDDEN_CLOBBER";
    case ASMKIT_ERR_FEATURE_DISABLED: return "ASMKIT_ERR_FEATURE_DISABLED";
    case ASMKIT_ERR_INTERNAL: return "ASMKIT_ERR_INTERNAL";
    default: return "ASMKIT_ERR_UNKNOWN";
    }
}

static const char* operand_kind_name(asmkit_operand_kind_t kind)
{
    switch (kind) {
    case ASMKIT_OP_NONE: return "none";
    case ASMKIT_OP_REG: return "reg";
    case ASMKIT_OP_IMM: return "imm";
    case ASMKIT_OP_MEM: return "mem";
    case ASMKIT_OP_PC_REL: return "pc-rel";
    case ASMKIT_OP_WASM_BLOCKTYPE: return "wasm-blocktype";
    case ASMKIT_OP_WASM_INDEX: return "wasm-index";
    case ASMKIT_OP_OPAQUE: return "opaque";
    case ASMKIT_OP_X86_ROUNDING: return "x86-rounding";
    default: return "unknown";
    }
}

static int parse_arch_mode(const char* arch_text, const char* mode_text, asmkit_arch_t* arch, asmkit_mode_t* mode)
{
    if (strcmp(arch_text, "x86") == 0) {
        *arch = ASMKIT_ARCH_X86;
        if (strcmp(mode_text, "16") == 0 || strcmp(mode_text, "x86_16") == 0) {
            *mode = ASMKIT_MODE_X86_16;
            return 1;
        }
        if (strcmp(mode_text, "32") == 0 || strcmp(mode_text, "x86_32") == 0) {
            *mode = ASMKIT_MODE_X86_32;
            return 1;
        }
        if (strcmp(mode_text, "64") == 0 || strcmp(mode_text, "x86_64") == 0 || strcmp(mode_text, "amd64") == 0) {
            *mode = ASMKIT_MODE_X86_64;
            return 1;
        }
    }
    if (strcmp(arch_text, "arm") == 0) {
        *arch = ASMKIT_ARCH_ARM;
        if (strcmp(mode_text, "a32") == 0 || strcmp(mode_text, "arm") == 0 || strcmp(mode_text, "32") == 0) {
            *mode = ASMKIT_MODE_ARM_A32;
            return 1;
        }
        if (strcmp(mode_text, "thumb") == 0 || strcmp(mode_text, "t32") == 0) {
            *mode = ASMKIT_MODE_ARM_THUMB;
            return 1;
        }
    }
    if (strcmp(arch_text, "aarch64") == 0 || strcmp(arch_text, "arm64") == 0) {
        *arch = ASMKIT_ARCH_AARCH64;
        if (strcmp(mode_text, "aarch64") == 0 || strcmp(mode_text, "arm64") == 0 || strcmp(mode_text, "64") == 0) {
            *mode = ASMKIT_MODE_AARCH64;
            return 1;
        }
    }
    return 0;
}

static void print_bytes(const uint8_t* bytes, uint32_t size)
{
    uint32_t i;
    for (i = 0u; i < size; ++i) {
        printf("%02x", (unsigned)bytes[i]);
    }
}

static void print_operand_sig(const asmkit_inst_t* inst)
{
    uint32_t i;
    for (i = 0u; i < inst->operand_count && i < ASMKIT_MAX_OPERANDS; ++i) {
        const asmkit_operand_t* op = &inst->operands[i];
        if (i != 0u) {
            putchar(';');
        }
        printf("%s,%u,0x%x,%llu,%lld,%llu,%llu,%u,%lld,%u,%llu",
            operand_kind_name(op->kind),
            (unsigned)op->width,
            (unsigned)op->flags,
            (unsigned long long)op->reg,
            (long long)op->imm,
            (unsigned long long)op->mem.base,
            (unsigned long long)op->mem.index,
            (unsigned)op->mem.scale,
            (long long)op->mem.displacement,
            (unsigned)op->mem.address_width,
            (unsigned long long)op->mem.segment);
    }
}

int main(int argc, char** argv)
{
    asmkit_arch_t arch;
    asmkit_mode_t mode;
    uint64_t address;
    size_t cap;
    uint8_t* code;
    size_t code_size;
    size_t offset = 0u;
    asmkit_decoder_t decoder;
    asmkit_formatter_t formatter;
    asmkit_status_t status;

    if (argc != 5) {
        fprintf(stderr, "usage: %s <arch> <mode> <hex-base-address> <hex-bytes>\n", argv[0]);
        return 2;
    }
    if (!parse_arch_mode(argv[1], argv[2], &arch, &mode)) {
        fprintf(stderr, "unsupported arch/mode: %s/%s\n", argv[1], argv[2]);
        return 2;
    }

    cap = strlen(argv[4]) / 2u + 1u;
    code = (uint8_t*)malloc(cap);
    if (code == NULL) {
        fprintf(stderr, "allocation failed\n");
        return 2;
    }
    address = _strtoui64(argv[3], 0, 0);
    code_size = parse_hex(argv[4], code, cap);
    if (code_size == 0u) {
        fprintf(stderr, "invalid byte string\n");
        free(code);
        return 2;
    }

    status = asmkit_decoder_init(&decoder, arch, mode);
    if (status != ASMKIT_OK) {
        fprintf(stderr, "decoder_init failed: %s (%d)\n", status_name(status), (int)status);
        free(code);
        return 1;
    }
    status = asmkit_formatter_init(&formatter, 0u);
    if (status != ASMKIT_OK) {
        fprintf(stderr, "formatter_init failed: %s (%d)\n", status_name(status), (int)status);
        free(code);
        return 1;
    }

    while (offset < code_size) {
        asmkit_inst_t inst;
        char text[256];
        asmkit_text_result_t text_result;
        status = asmkit_decoder_decode_full(&decoder, code + offset, code_size - offset, address + offset, &inst);
        if (status != ASMKIT_OK) {
            printf("E\t%016llx\t%llu\t%s\t%d\t%02x\n",
                (unsigned long long)(address + offset),
                (unsigned long long)offset,
                status_name(status),
                (int)status,
                (unsigned)code[offset]);
            free(code);
            return 0;
        }
        status = asmkit_formatter_format_inst(&formatter, &inst, text, sizeof(text), &text_result);
        if (status != ASMKIT_OK) {
            snprintf(text, sizeof(text), "<format %s>", status_name(status));
        }
        printf("I\t%016llx\t%llu\t%u\t",
            (unsigned long long)inst.address,
            (unsigned long long)offset,
            (unsigned)inst.size);
        print_bytes(inst.bytes, inst.size);
        printf("\t%u\t%u\t%u\t%u\t0x%x\t0x%x\t%s\t",
            (unsigned)inst.id,
            (unsigned)inst.mnemonic_id,
            (unsigned)inst.inst_class,
            (unsigned)inst.operand_count,
            (unsigned)inst.flags,
            (unsigned)inst.attributes,
            text);
        print_operand_sig(&inst);
        putchar('\n');
        offset += inst.size;
    }

    free(code);
    return 0;
}
