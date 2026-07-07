/* CC1_FLAGS: -g3 */
typedef signed char s8;
typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;
typedef int s32;
typedef unsigned int u32;
typedef long long s64;
#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"
M2C_UNK Render_SetGteScreenOffset();
M2C_UNK Pm_Start();
M2C_UNK Render_ResetGteScreenOffset();
M2C_UNK Pm_Exec();
extern s32 g_GameStateFlags[];
#define g_GameStateFlags (g_GameStateFlags[0])
extern s32 g_PmSlotTable[];
#define g_PmSlotTable (g_PmSlotTable[0])
s32 Pm_RunPrimary(void) {
    s32 var_s0;
    s32 var_s0_2;
    s32 var_s1;
    s32 temp_v1;

    if (g_GameStateFlags & 0x80) {
        var_s0 = 0;
        Render_SetGteScreenOffset();
        do {
            Pm_Start(var_s0);
            var_s0 += 1;
        } while (var_s0 < 0xB);
        Render_ResetGteScreenOffset();
        if (!(g_GameStateFlags & 4)) {
            var_s0_2 = 0;
            var_s1 = 0;
            do {
                temp_v1 = M2C_FIELD((var_s1 + g_PmSlotTable), u8 *, 1);
                if (!(g_GameStateFlags & 0x100) || ((u32) (temp_v1 - 0x55) < 0x1EU)) {
                    Pm_Exec(var_s0_2);
                }
                var_s0_2 += 1;
                var_s1 += 0xA0C;
            } while (var_s0_2 < 0xB);
        }
    }
    return 0;
}
