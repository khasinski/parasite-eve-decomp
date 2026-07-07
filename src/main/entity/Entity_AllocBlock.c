/* CC1_FLAGS: -g3 */
typedef signed char s8;typedef unsigned char u8;typedef short s16;typedef unsigned short u16;typedef int s32;typedef unsigned int u32;typedef long long s64;

extern char D_800A7620[];
extern char g_EntityAllocBlockTable[];
extern s32 g_RenderScratchBufferBase;

#define LOAD_BASEPTR(dst)                                             \
    asm volatile(                                                     \
        "lui\t%0, %%hi(g_RenderScratchBufferBase)\n"                                 \
        "addiu\t%0, %0, %%lo(g_RenderScratchBufferBase)"                             \
        : "=r"(dst))

s32 Entity_AllocBlock(u32 arg0) {
    s32 temp_a0;
    u32 var_v0;
    s32 var_a1;
    s32 var_a2;
    register s32 var_a3 asm("$7");
    register u32 temp_t0 asm("$8");
    s32 *temp_t1;
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
    LOAD_BASEPTR(temp_t1);
    var_a0 = var_a1 & 0xFF;
loop_8:
    var_v1 = var_a0 << 3;
    if (*(s32 *)(g_EntityAllocBlockTable + var_v1) == 0) {
        var_a2 = 1;
        if (var_a0 < (u32)(var_a0 + temp_t0)) {
            var_v0 = var_a3 + var_a0;
            var_a0 = var_v0 << 3;
loop_11:
            if (*(s32 *)(g_EntityAllocBlockTable + var_v1) != 0) {
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
            *(u32 *)(D_800A7620 + temp_a0) = temp_t0;
            var_v0 = *temp_t1 + (var_v1 << 5);
            *(s32 *)(g_EntityAllocBlockTable + temp_a0) = var_v0;
            return var_v0;
        }
        goto block_20;
occupied_in_run:
        var_a2 = 0;
        var_a1 += var_a3;
        goto after_empty_scan;
    }
    var_v1 = *(u32 *)(D_800A7620 + var_v1);
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
