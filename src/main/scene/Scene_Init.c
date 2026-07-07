/* CC1_FLAGS: -g3 */
typedef signed char s8; typedef unsigned char u8; typedef short s16;
typedef unsigned short u16; typedef int s32; typedef unsigned int u32; typedef long long s64;
#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"
M2C_UNK Scene_SyncEntityAction();
M2C_UNK Entity_SetActionMode();
extern s32 g_GameStateFlags[];
#define g_GameStateFlags (g_GameStateFlags[0])

void Scene_Init(void *arg0, s32 *arg1) {
    if (g_GameStateFlags & 2) {
        Scene_SyncEntityAction();
        return;
    }
    if (*arg1 != 0x15) {
        M2C_FIELD(arg0, s32 *, 0x68) = 0;
        M2C_FIELD(arg0, s32 *, 0x6C) = 0;
        M2C_FIELD(arg0, s32 *, 0x70) = 0;
        Entity_SetActionMode(arg0, 0x15);
        *arg1 = 0x15;
    }
}
