/* CC1_FLAGS: -g3 -G8 */
/* MASPSX_FLAGS: -G8 */
typedef signed char s8;typedef unsigned char u8;typedef short s16;typedef unsigned short u16;typedef int s32;typedef unsigned int u32;typedef long long s64;
#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"
M2C_UNK ClearImage();
M2C_UNK Render_Noop();
M2C_UNK Asset_UnloadTableEntries();
M2C_UNK DrawSync();
M2C_UNK Akao_Cmd_F1();
extern s32 g_GameStateFlags;
extern s32 g_GameState[];
#define g_GameState (g_GameState[0])

void Boot_FlushSceneFast(void) {
    s16 rect[4];
    s32 temp_v0;
    s32 temp_v1;

    if (g_GameState & 0x200) {
        rect[2] = 0x140;
        rect[0] = 0;
        rect[1] = 0;
        rect[3] = 0x1C0;
        ClearImage(rect, 0, 0, 1);
    }
    DrawSync(0);
    Akao_Cmd_F1();
    Render_Noop(1);
    Asset_UnloadTableEntries();
    temp_v1 = g_GameState | 2;
    g_GameStateFlags = (g_GameStateFlags | 0x40) & ~0x3800;
    temp_v0 = temp_v1 & ~0x800;
    g_GameState = temp_v0;
    if (temp_v1 & 0x200) {
        g_GameState = (temp_v0 | 2) & 0xFFFF7DFF;
    }
}
