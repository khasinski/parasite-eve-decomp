/* CC1_FLAGS: -g3 */
/* MASPSX_FLAGS: -G8 */
typedef signed char s8;typedef unsigned char u8;typedef short s16;typedef unsigned short u16;typedef int s32;typedef unsigned int u32;typedef long long s64;
#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"
M2C_UNK Entity_SetActionMode();
M2C_UNK Entity_SetAction();
extern s32 g_GameStateFlags[];
#define g_GameStateFlags (g_GameStateFlags[0])

void Scene_SyncEntityAction(void *arg0, s32 *arg1) {
    u8 temp_a1;

    if (!(g_GameStateFlags & 2)) {
        if (*arg1 != 0x15) {
            M2C_FIELD(arg0, s32 *, 0x68) = 0;
            M2C_FIELD(arg0, s32 *, 0x6C) = 0;
            M2C_FIELD(arg0, s32 *, 0x70) = 0;
            Entity_SetActionMode(arg0, 0x15);
            *arg1 = 0x15;
        }
    } else {
        temp_a1 = M2C_FIELD(M2C_FIELD(arg0, void **, 0), u8 *, 0x12);
        if (*arg1 != temp_a1) {
            Entity_SetAction(arg0, temp_a1);
            M2C_FIELD(arg0, s32 *, 0x68) = 0;
            M2C_FIELD(arg0, s32 *, 0x6C) = 0;
            M2C_FIELD(arg0, s32 *, 0x70) = 0;
            *arg1 = (s32) M2C_FIELD(M2C_FIELD(arg0, void **, 0), u8 *, 0x12);
        }
    }
}
