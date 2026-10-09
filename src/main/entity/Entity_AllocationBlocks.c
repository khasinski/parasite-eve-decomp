#include "pe1/entity_allocation.h"
extern s32 g_RenderScratchBufferBase;

s32 Entity_AllocBlock(u32 arg0) {
    s32 temp_a0;
    u32 var_v0;
    s32 var_a1;
    s32 var_a2;
    register s32 var_a3 asm("$7");
    register u32 temp_t0 asm("$8");
    register s32 *temp_t1 asm("$9");
    u32 var_a0;
    register u32 var_v1 asm("$3");

    var_a3 = 8;
    if (arg0 < 0x47E1U) {
        var_a3 = 1;
        goto block_6;
    }
    if (arg0 <= 0x8FC0U) {
        var_a3 = 2;
        goto block_6;
    }
    var_a1 = 0;
    if (arg0 <= 0x11F80U) {
        var_a3 = 4;
block_6:
        var_a1 = 0;
    }
    temp_t0 = var_a3 & 0xFF;
    temp_t1 = &g_RenderScratchBufferBase;
    var_a0 = var_a1 & 0xFF;
loop_8:
    var_v1 = var_a0 << 3;
    if (D_800A7620[var_a0].address == 0) {
        var_a2 = 1;
        if (var_a0 < (var_a0 + temp_t0)) {
            var_v0 = var_a3 + var_a0;
            var_a0 = var_v0 << 3;
loop_11:
            if (((EntityAllocationBlock *)((u8 *)D_800A7620 + var_v1))->address != 0) {
                goto occupied_in_run;
            }
            var_v1 += 8;
            if (var_v1 < var_a0) {
                goto loop_11;
            }
        }
after_empty_scan:
        var_v0 = var_a1 & 0xFF;
        if (var_a2 != 0) {
            temp_a0 = var_v0 << 3;
            var_v1 = temp_a0 + var_v0;
            var_v1 = (var_v1 << 6) - var_v0;
            D_800A7620[var_v0].blockCount = temp_t0;
            var_v0 = *temp_t1 + (var_v1 << 5);
            ((EntityAllocationBlock *)((u8 *)D_800A7620 + temp_a0))->address = var_v0;
            return var_v0;
        }
        goto block_20;
occupied_in_run:
        var_a2 = 0;
        var_a1 += var_a3;
        goto after_empty_scan;
    }
    var_v1 = ((EntityAllocationBlock *)((u8 *)D_800A7620 + var_v1))->blockCount;
    if (temp_t0 < var_v1) {
        var_a1 += var_v1;
    } else {
        var_a1 += var_a3;
    }
    var_v0 = var_a1 & 0xFF;
block_20:
    var_a0 = var_a1 & 0xFF;
    if (var_v0 >= 0x10U) {
        return 0;
    }
    goto loop_8;
}


void Entity_FreeAllocationBlock(int arg0) {
    int i;

    i = 0;
    while ((unsigned int)(i & 0xFF) < 0x10U) {
        if (D_800A7620[i & 0xFF].address == arg0) {
            D_800A7620[i & 0xFF].address = 0;
            return;
        }
        i++;
    }
}
