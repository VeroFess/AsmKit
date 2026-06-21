#include "test_support.h"

int asmkit_test_relocate(void)
{
    asmkit_engine_t engine;
    asmkit_relocate_options_t options;
    asmkit_detour_plan_options_t plan_options;
    asmkit_detour_plan_t plan;
    asmkit_relocate_result_t result;
    uint8_t out[64];
    uint64_t a64_x16_mask;
    uint8_t prologue[] = {0x55u, 0x48u, 0x89u, 0xe5u};
    uint8_t riprel[] = {0x48u, 0x8bu, 0x05u, 0x00u, 0x00u, 0x00u, 0x00u};
    uint8_t x86_call[] = {0xe8u, 0x00u, 0x00u, 0x00u, 0x00u};
    uint8_t x86_jmp[] = {0xe9u, 0x00u, 0x00u, 0x00u, 0x00u};
    uint8_t x86_jne[] = {0x75u, 0x00u};
    uint8_t x86_jne_block[] = {0x75u, 0x00u, 0x90u, 0x90u, 0x90u};
    uint8_t arm_b[] = {0x00u, 0x00u, 0x00u, 0xeau};
    uint8_t arm_bl[] = {0x00u, 0x00u, 0x00u, 0xebu};
    uint64_t arm_ip_mask;
    uint8_t thumb_it[] = {0x08u, 0xbfu, 0x00u, 0xbfu};
    uint8_t a64_b[] = {0x04u, 0x00u, 0x00u, 0x14u};
    uint8_t a64_beq[] = {0x40u, 0x00u, 0x00u, 0x54u};
    uint8_t a64_nop[] = {0x1fu, 0x20u, 0x03u, 0xd5u};
    uint8_t bpf_exit[] = {0x95u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u};
    uint8_t wasm_return[] = {0x0fu};

    ASMKIT_INIT_ENGINE(&engine, ASMKIT_ARCH_X86, ASMKIT_MODE_X86_64);
    options.min_overwrite_size = 4u;
    options.continuation_address = 0x1004u;
    options.jump_back_mode = ASMKIT_BRANCH_DIRECT_ONLY;
    options.allowed_clobber_mask_lo = 0u;
    options.allowed_clobber_mask_hi = 0u;
    options.flags = 0u;

    ASMKIT_CHECK(asmkit_relocate_prologue(&engine, 0, prologue, sizeof(prologue), 0x1000u, 0x2000u, &options, out, sizeof(out), &result) == ASMKIT_OK);
    ASMKIT_CHECK(result.overwritten_size == 4u);
    ASMKIT_CHECK(result.relocated_size == 9u);
    ASMKIT_CHECK(result.relocated_inst_count == 2u);
    ASMKIT_CHECK(result.relocated_insts[0].original_address == 0x1000u);
    ASMKIT_CHECK(result.relocated_insts[0].original_size == 1u);
    ASMKIT_CHECK(result.relocated_insts[0].relocated_address == 0x2000u);
    ASMKIT_CHECK(result.relocated_insts[0].relocated_offset == 0u);
    ASMKIT_CHECK(result.relocated_insts[0].relocated_size == 1u);
    ASMKIT_CHECK(result.relocated_insts[1].original_address == 0x1001u);
    ASMKIT_CHECK(result.relocated_insts[1].original_size == 3u);
    ASMKIT_CHECK(result.relocated_insts[1].relocated_address == 0x2001u);
    ASMKIT_CHECK(result.relocated_insts[1].relocated_offset == 1u);
    ASMKIT_CHECK(result.relocated_insts[1].relocated_size == 3u);
    ASMKIT_CHECK(out[0] == 0x55u && out[1] == 0x48u && out[2] == 0x89u && out[3] == 0xe5u);
    ASMKIT_CHECK(out[4] == 0xe9u);

    options.min_overwrite_size = 7u;
    options.continuation_address = 0x1007u;
    ASMKIT_CHECK(asmkit_relocate_prologue(&engine, 0, riprel, sizeof(riprel), 0x1000u, 0x2000u, &options, out, sizeof(out), &result) == ASMKIT_OK);
    ASMKIT_CHECK(result.overwritten_size == 7u);
    ASMKIT_CHECK((int32_t)asmkit_test_load32le(out + 3) == -4096);

    options.min_overwrite_size = 5u;
    options.continuation_address = 0x1005u;
    options.jump_back_mode = ASMKIT_BRANCH_AUTO;
    ASMKIT_CHECK(asmkit_relocate_prologue(&engine, 0, x86_call, sizeof(x86_call), 0x1000u, 0x1000000000ull, &options, out, sizeof(out), &result) == ASMKIT_OK);
    ASMKIT_CHECK(result.overwritten_size == 5u);
    ASMKIT_CHECK(result.jump_back_offset == 14u);
    ASMKIT_CHECK(result.relocated_size == 28u);
    ASMKIT_CHECK(out[0] == 0xffu && out[1] == 0x15u);
    ASMKIT_CHECK(asmkit_test_load32le(out + 2) == 0u);
    ASMKIT_CHECK(asmkit_test_load64le(out + 6) == 0x1005u);
    ASMKIT_CHECK(out[14] == 0xffu && out[15] == 0x25u);

    ASMKIT_CHECK(asmkit_relocate_prologue(&engine, 0, x86_jmp, sizeof(x86_jmp), 0x1000u, 0x1000000000ull, &options, out, sizeof(out), &result) == ASMKIT_OK);
    ASMKIT_CHECK(result.jump_back_offset == 14u);
    ASMKIT_CHECK(result.relocated_size == 28u);
    ASMKIT_CHECK(out[0] == 0xffu && out[1] == 0x25u);
    ASMKIT_CHECK(asmkit_test_load64le(out + 6) == 0x1005u);

    plan_options.branch_mode = ASMKIT_BRANCH_DIRECT_ONLY;
    plan_options.min_patch_size_override = 0u;
    plan_options.flags = 0u;
    ASMKIT_CHECK(asmkit_plan_detour(&engine, 0, x86_jne_block, sizeof(x86_jne_block), 0x1000u, 0x2000u, &plan_options, &plan) == ASMKIT_OK);
    ASMKIT_CHECK(plan.patch_size == 5u);
    ASMKIT_CHECK(plan.trampoline_min_size == 24u);
    ASMKIT_CHECK(plan.requires_near_island);
    ASMKIT_CHECK(plan.island_min_distance == (int64_t)(-2147483647 - 1));
    ASMKIT_CHECK(plan.island_max_distance == 2147483647);

    plan_options.branch_mode = ASMKIT_BRANCH_AUTO;
    ASMKIT_CHECK(asmkit_plan_detour(&engine, 0, x86_jne_block, sizeof(x86_jne_block), 0x1000u, 0x2000u, &plan_options, &plan) == ASMKIT_OK);
    ASMKIT_CHECK(plan.patch_size == 5u);
    ASMKIT_CHECK(plan.trampoline_min_size == 33u);
    ASMKIT_CHECK(!plan.requires_near_island);

    options.min_overwrite_size = 2u;
    options.continuation_address = 0x1002u;
    options.jump_back_mode = ASMKIT_BRANCH_AUTO;
    ASMKIT_CHECK(asmkit_relocate_prologue(&engine, 0, x86_jne, sizeof(x86_jne), 0x1000u, 0x1000000000ull, &options, out, sizeof(out), &result) == ASMKIT_OK);
    ASMKIT_CHECK(result.overwritten_size == 2u);
    ASMKIT_CHECK(result.jump_back_offset == 16u);
    ASMKIT_CHECK(result.relocated_size == 30u);
    ASMKIT_CHECK(result.relocated_inst_count == 1u);
    ASMKIT_CHECK(result.relocated_insts[0].original_address == 0x1000u);
    ASMKIT_CHECK(result.relocated_insts[0].original_size == 2u);
    ASMKIT_CHECK(result.relocated_insts[0].relocated_address == 0x1000000000ull);
    ASMKIT_CHECK(result.relocated_insts[0].relocated_offset == 0u);
    ASMKIT_CHECK(result.relocated_insts[0].relocated_size == 16u);
    ASMKIT_CHECK(out[0] == 0x74u && out[1] == 14u);
    ASMKIT_CHECK(out[2] == 0xffu && out[3] == 0x25u);
    ASMKIT_CHECK(asmkit_test_load32le(out + 4) == 0u);
    ASMKIT_CHECK(asmkit_test_load64le(out + 8) == 0x1002u);
    ASMKIT_CHECK(out[16] == 0xffu && out[17] == 0x25u);

    ASMKIT_INIT_ENGINE(&engine, ASMKIT_ARCH_ARM, ASMKIT_MODE_ARM_A32);
    arm_ip_mask = (uint64_t)1u << 12;
    options.min_overwrite_size = 4u;
    options.continuation_address = 0x1004u;
    options.jump_back_mode = ASMKIT_BRANCH_AUTO;
    options.allowed_clobber_mask_lo = 0u;
    options.allowed_clobber_mask_hi = 0u;
    ASMKIT_CHECK(asmkit_relocate_prologue(&engine, 0, arm_b, sizeof(arm_b), 0x1000u, 0x1000000000ull, &options, out, sizeof(out), &result) == ASMKIT_OK);
    ASMKIT_CHECK(result.overwritten_size == 4u);
    ASMKIT_CHECK(result.jump_back_offset == 8u);
    ASMKIT_CHECK(result.jump_back_size == 8u);
    ASMKIT_CHECK(result.relocated_size == 16u);
    ASMKIT_CHECK(asmkit_test_load32le(out) == 0xe51ff004u);
    ASMKIT_CHECK(asmkit_test_load32le(out + 4) == 0x1008u);
    ASMKIT_CHECK(asmkit_test_load32le(out + 8) == 0xe51ff004u);
    ASMKIT_CHECK(asmkit_test_load32le(out + 12) == 0x1004u);

    ASMKIT_CHECK(asmkit_relocate_prologue(&engine, 0, arm_bl, sizeof(arm_bl), 0x1000u, 0x1000000000ull, &options, out, sizeof(out), &result) == ASMKIT_ERR_FORBIDDEN_CLOBBER);
    ASMKIT_CHECK((result.clobber_mask_lo & arm_ip_mask) != 0u);
    options.allowed_clobber_mask_lo = arm_ip_mask;
    ASMKIT_CHECK(asmkit_relocate_prologue(&engine, 0, arm_bl, sizeof(arm_bl), 0x1000u, 0x1000000000ull, &options, out, sizeof(out), &result) == ASMKIT_OK);
    ASMKIT_CHECK(result.overwritten_size == 4u);
    ASMKIT_CHECK(result.jump_back_offset == 12u);
    ASMKIT_CHECK(result.jump_back_size == 8u);
    ASMKIT_CHECK(result.relocated_size == 20u);
    ASMKIT_CHECK(asmkit_test_load32le(out) == 0xe59fc004u);
    ASMKIT_CHECK(asmkit_test_load32le(out + 4) == 0xe12fff3cu);
    ASMKIT_CHECK(asmkit_test_load32le(out + 8) == 0x1008u);
    ASMKIT_CHECK(asmkit_test_load32le(out + 12) == 0xe51ff004u);
    ASMKIT_CHECK(asmkit_test_load32le(out + 16) == 0x1004u);

    plan_options.branch_mode = ASMKIT_BRANCH_AUTO;
    plan_options.min_patch_size_override = 0u;
    plan_options.flags = 0u;
    ASMKIT_CHECK(asmkit_plan_detour(&engine, 0, arm_b, sizeof(arm_b), 0x1000u, 0x2000u, &plan_options, &plan) == ASMKIT_OK);
    ASMKIT_CHECK(plan.patch_size == 4u);
    ASMKIT_CHECK(plan.trampoline_min_size == 20u);
    ASMKIT_CHECK(!plan.requires_near_island);

    ASMKIT_INIT_ENGINE(&engine, ASMKIT_ARCH_ARM, ASMKIT_MODE_ARM_THUMB);
    plan_options.branch_mode = ASMKIT_BRANCH_DIRECT_ONLY;
    plan_options.min_patch_size_override = 0u;
    plan_options.flags = 0u;
    ASMKIT_CHECK(asmkit_plan_detour(&engine, 0, thumb_it, sizeof(thumb_it), 0x1000u, 0x1004u, &plan_options, &plan) == ASMKIT_ERR_UNSUPPORTED_RELOCATION);
    ASMKIT_CHECK((plan.hazard_flags & ASMKIT_HAZARD_ARM_IT) != 0u);

    options.min_overwrite_size = 2u;
    options.continuation_address = 0x1002u;
    options.jump_back_mode = ASMKIT_BRANCH_DIRECT_ONLY;
    ASMKIT_CHECK(asmkit_relocate_prologue(&engine, 0, thumb_it, sizeof(thumb_it), 0x1000u, 0x2000u, &options, out, sizeof(out), &result) == ASMKIT_ERR_UNSUPPORTED_RELOCATION);
    ASMKIT_CHECK((result.hazard_flags & ASMKIT_HAZARD_ARM_IT) != 0u);

    ASMKIT_INIT_ENGINE(&engine, ASMKIT_ARCH_AARCH64, ASMKIT_MODE_AARCH64);
    a64_x16_mask = (uint64_t)1u << 16;
    plan_options.branch_mode = ASMKIT_BRANCH_DIRECT_ONLY;
    plan_options.min_patch_size_override = 0u;
    plan_options.flags = 0u;
    ASMKIT_CHECK(asmkit_plan_detour(&engine, 0, a64_beq, sizeof(a64_beq), 0x1000u, 0x2000u, &plan_options, &plan) == ASMKIT_OK);
    ASMKIT_CHECK(plan.patch_size == 4u);
    ASMKIT_CHECK(plan.trampoline_min_size == 24u);
    ASMKIT_CHECK(plan.requires_near_island);
    ASMKIT_CHECK(plan.island_min_distance == -134217728);
    ASMKIT_CHECK(plan.island_max_distance == 134217724);

    plan_options.branch_mode = ASMKIT_BRANCH_AUTO;
    ASMKIT_CHECK(asmkit_plan_detour(&engine, 0, a64_beq, sizeof(a64_beq), 0x1000u, 0x2000u, &plan_options, &plan) == ASMKIT_OK);
    ASMKIT_CHECK(plan.patch_size == 4u);
    ASMKIT_CHECK(plan.trampoline_min_size == 36u);
    ASMKIT_CHECK(!plan.requires_near_island);
    ASMKIT_CHECK((plan.clobber_mask_lo & a64_x16_mask) != 0u);

    options.min_overwrite_size = 4u;
    options.continuation_address = 0x1004u;
    options.jump_back_mode = ASMKIT_BRANCH_ALLOW_SCRATCH_CLOBBER;
    options.allowed_clobber_mask_lo = a64_x16_mask;
    options.allowed_clobber_mask_hi = 0u;
    ASMKIT_CHECK(asmkit_relocate_prologue(&engine, 0, a64_b, sizeof(a64_b), 0x1000u, 0x1000000000ull, &options, out, sizeof(out), &result) == ASMKIT_OK);
    ASMKIT_CHECK(result.overwritten_size == 4u);
    ASMKIT_CHECK(result.jump_back_offset == 16u);
    ASMKIT_CHECK(result.relocated_size == 32u);
    ASMKIT_CHECK(asmkit_test_load32le(out) == 0x58000050u);
    ASMKIT_CHECK(asmkit_test_load32le(out + 4) == 0xd61f0200u);
    ASMKIT_CHECK(asmkit_test_load64le(out + 8) == 0x1010u);
    ASMKIT_CHECK((result.clobber_mask_lo & a64_x16_mask) != 0u);

    options.allowed_clobber_mask_lo = 0u;
    ASMKIT_CHECK(asmkit_relocate_prologue(&engine, 0, a64_b, sizeof(a64_b), 0x1000u, 0x1000000000ull, &options, out, sizeof(out), &result) == ASMKIT_ERR_FORBIDDEN_CLOBBER);

    options.allowed_clobber_mask_lo = a64_x16_mask;
    ASMKIT_CHECK(asmkit_relocate_prologue(&engine, 0, a64_beq, sizeof(a64_beq), 0x1000u, 0x1000000000ull, &options, out, sizeof(out), &result) == ASMKIT_OK);
    ASMKIT_CHECK(result.jump_back_offset == 20u);
    ASMKIT_CHECK(result.relocated_size == 36u);
    ASMKIT_CHECK(asmkit_test_load32le(out) == 0x540000a1u);
    ASMKIT_CHECK(asmkit_test_load32le(out + 4) == 0x58000050u);
    ASMKIT_CHECK(asmkit_test_load64le(out + 12) == 0x1008u);

    options.min_overwrite_size = 4u;
    options.continuation_address = 0x1004u;
    options.jump_back_mode = ASMKIT_BRANCH_AUTO;
    options.allowed_clobber_mask_lo = 0u;
    ASMKIT_CHECK(asmkit_relocate_prologue(&engine, 0, a64_nop, sizeof(a64_nop), 0x1000u, 0x1000000000ull, &options, out, sizeof(out), &result) == ASMKIT_ERR_FORBIDDEN_CLOBBER);
    ASMKIT_CHECK((result.clobber_mask_lo & a64_x16_mask) != 0u);
    options.allowed_clobber_mask_lo = a64_x16_mask;
    ASMKIT_CHECK(asmkit_relocate_prologue(&engine, 0, a64_nop, sizeof(a64_nop), 0x1000u, 0x1000000000ull, &options, out, sizeof(out), &result) == ASMKIT_OK);
    ASMKIT_CHECK(result.jump_back_offset == 4u);
    ASMKIT_CHECK(result.jump_back_size == 16u);
    ASMKIT_CHECK(result.relocated_size == 20u);

    ASMKIT_INIT_ENGINE(&engine, ASMKIT_ARCH_BPF, ASMKIT_MODE_BPF64);
    ASMKIT_CHECK(asmkit_plan_detour(&engine, 0, bpf_exit, sizeof(bpf_exit), 0x1000u, 0x2000u, &plan_options, &plan) == ASMKIT_ERR_UNSUPPORTED_TARGET_OPERATION);
    ASMKIT_CHECK(asmkit_relocate_prologue(&engine, 0, bpf_exit, sizeof(bpf_exit), 0x1000u, 0x2000u, &options, out, sizeof(out), &result) == ASMKIT_ERR_UNSUPPORTED_TARGET_OPERATION);

    ASMKIT_INIT_ENGINE(&engine, ASMKIT_ARCH_WASM, ASMKIT_MODE_WASM32);
    ASMKIT_CHECK(asmkit_plan_detour(&engine, 0, wasm_return, sizeof(wasm_return), 0u, 0u, &plan_options, &plan) == ASMKIT_ERR_UNSUPPORTED_TARGET_OPERATION);
    ASMKIT_CHECK(asmkit_relocate_prologue(&engine, 0, wasm_return, sizeof(wasm_return), 0u, 0u, &options, out, sizeof(out), &result) == ASMKIT_ERR_UNSUPPORTED_TARGET_OPERATION);

    return 0;
}
