/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
typedef signed char s8;typedef unsigned char u8;typedef short s16;typedef unsigned short u16;typedef int s32;typedef unsigned int u32;typedef long long s64;
#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"
M2C_UNK MemCard_InitSlotState();
M2C_UNK Battle_StartEncounter();
M2C_UNK func_80042F20();
s32 Asset_LoadTimTextures();
s32 CD_StepReadState();
extern s32 g_SceneDataTable0;
extern struct { char _[16]; } D_8009D1A0_o __asm__("g_GameStateFlags");
extern s32 D_8009D1A0_rd[] __asm__("g_GameStateFlags");
#define g_GameStateFlags (*(s32 *)&D_8009D1A0_o)
extern void *g_TaskNodePool;
extern struct { char _[16]; } D_800B0CD8_o __asm__("g_GameState");
#define CD8_W (*(s32 *)base)
#define CD8_WABS (*(s32 *)&D_800B0CD8_o)
#define CD8_B(off) (*(base + (off)))

s32 Boot_MemCardStepInit(u8 **arg0) {
    u8 *base;

    base = (u8 *)&D_800B0CD8_o;
    __asm__("" : "=r"(base) : "0"(base));
loop_1:
    switch (CD8_B(0xF4)) {
    case 0x0:
        if (!(CD8_B(0xE) & 3)) {
            CD8_B(0xF4) = 0x37U;
        }
        CD8_W = CD8_W | 0x800000;
        goto block_19;
    case 0x37:
        if (!(CD8_WABS & 0x400000)) {
            MemCard_InitSlotState();
        }
        CD8_B(0xF4) = 0x38U;
        goto loop_1;
    case 0x38:
        if (CD_StepReadState(1) != 1) {
            if (!(CD8_WABS & 0x400000)) {
                func_80042F20();
            }
            CD8_B(0xF4) = 0x39U;
            goto loop_1;
        }
        goto block_19;
    case 0x39:
        if (Asset_LoadTimTextures(1) != 1) {
            CD8_B(0xF4) = 0x3AU;
            goto loop_1;
        }
        goto block_19;
    case 0x3A:
        {
            s32 t1a0 = D_8009D1A0_rd[0] | 2;
            g_GameStateFlags = t1a0;
        }
        Battle_StartEncounter(**arg0);
        CD8_B(0xF4) = 0x3BU;
        goto block_19;
    case 0x3B:
        if (!(CD8_B(0xE) & 3)) {
            CD8_B(0xF4) = 0U;
            CD8_W = CD8_W & 0xFF7FFFFF;
            __asm__ volatile("");
            return 1;
        }
        break;
    default:
        return 1;
    }
block_19:
    g_SceneDataTable0 -= 0xC;
    M2C_FIELD(g_TaskNodePool, s32 *, 0x10) = 1;
    return 0;
}
