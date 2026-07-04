/* CC1_FLAGS: -g3 -G8 */
/* MASPSX_FLAGS: -G8 */
typedef signed char s8;typedef unsigned char u8;typedef short s16;typedef unsigned short u16;typedef int s32;typedef unsigned int u32;typedef long long s64;
#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"
s32 Menu_IsEquipSlotActive();
M2C_UNK Render_LoadFontGlyphAlt();
s32 Menu_GetEquipSlotStateOrIndex();
s32 Render_SetFontGlyphByCode();
M2C_UNK Menu_ResetEquipSlotState();
s32 func_8006E3D4();
extern M2C_UNK g_MapSelectIndexTable[];
#define g_MapSelectIndexTable (g_MapSelectIndexTable[0])
extern M2C_UNK g_MapFilenameTable[];
#define g_MapFilenameTable (g_MapFilenameTable[0])
extern M2C_UNK g_MapNameOverrideA[];
#define g_MapNameOverrideA (g_MapNameOverrideA[0])
extern M2C_UNK g_MapNameOverrideB[];
#define g_MapNameOverrideB (g_MapNameOverrideB[0])
extern s32 D_8009D1A0_r[] __asm__("g_GameStateFlags");
extern s32 D_8009D1A0_w[] __asm__("g_GameStateFlags");
extern s32 g_SceneDispatchCur[];
#define g_SceneDispatchCur (g_SceneDispatchCur[0])
extern s32 g_SceneDispatchToken[];
#define g_SceneDispatchToken (g_SceneDispatchToken[0])
extern s32 g_GameState[];
#define g_GameState (g_GameState[0])

s32 Task_SetFloorByEntityRoll(u8 **arg0) {
    M2C_UNK *var_a0;
    register s32 *temp_v1_2 asm("$3");
    register s32 temp_a0 asm("$4");
    register s32 temp_a1 asm("$5");
    u8 *temp_v1_3;
    s32 temp_s0;
    s32 temp_v0;
    register u32 temp_v0_3 asm("$2");
    u32 temp_v1_4;

    if (!(Menu_IsEquipSlotActive() & 0xFF)) {
        Render_LoadFontGlyphAlt();
    }
    temp_s0 = Render_SetFontGlyphByCode(**arg0) & 0xFF;
    if (temp_s0 == 0xFF) {
        if ((u32) (Menu_GetEquipSlotStateOrIndex() & 0xFF) < 2U) {
            Menu_ResetEquipSlotState();
            var_a0 = &g_MapNameOverrideA;
        } else {
            Menu_ResetEquipSlotState();
            var_a0 = &g_MapNameOverrideB;
        }
        g_SceneDispatchToken = func_8006E3D4(var_a0);
        return 1;
    }
    temp_v1_4 = (Menu_GetEquipSlotStateOrIndex() - 1) & 0xFF;
    temp_v0_3 = temp_v1_4 / 10U;
    temp_v1_4 = temp_v0_3 & 0xFF;
    temp_v0_3 = temp_v1_4 * 0x18;
    temp_v1_3 = (u8 *) &g_MapSelectIndexTable;
    temp_v0 = func_8006E3D4((*(temp_v1_3 + temp_v0_3 + temp_s0) * 8) + (u8 *) &g_MapFilenameTable);
    g_SceneDispatchToken = temp_v0;
    if (g_SceneDispatchCur == temp_v0) {
        temp_v1_2 = &g_GameState;
        temp_a0 = D_8009D1A0_r[0];
        temp_a1 = *temp_v1_2;
        D_8009D1A0_w[0] = temp_a0 | 0x2000;
        *temp_v1_2 = temp_a1 | 0x800;
    }
    return 1;
}
