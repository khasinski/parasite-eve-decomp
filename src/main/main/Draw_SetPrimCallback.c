/* CC1_FLAGS: -g3 -G8 */
/* MASPSX_FLAGS: -G8 --expand-div */
typedef signed char s8;typedef unsigned char u8;typedef short s16;typedef unsigned short u16;typedef int s32;typedef unsigned int u32;typedef long long s64;
#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"
M2C_UNK Draw_SwapPrimBuffers();
M2C_UNK MenuWidget_DestroyPopupNode();

void Draw_SetPrimCallback(void *arg0, s32 arg1, s32 arg3) {
    s32 temp_a1_2;
    s32 temp_a2;
    s32 temp_v1;
    s32 t58b;
    s32 t48;
    s32 t5C;
    s32 temp_v1_2;
    s32 temp_v1_3;
    s32 temp_v1_4;
    s32 var_a1;
    s32 var_a2;
    void *temp_a1;

    temp_v1 = M2C_FIELD(arg0, s32 *, 0x34);
    M2C_FIELD(arg0, s32 *, 0x58) = (s32) ((s32) ((arg1 + temp_v1) - 1) / temp_v1);
    var_a2 = 0;
    if (M2C_FIELD(arg0, s32 *, 0x34) == 2) {
        var_a2 = arg1 & 1;
    }
    __asm__ volatile("" : : : "memory");
    t48 = M2C_FIELD(arg0, s32 *, 0x48);
    temp_v1_2 = M2C_FIELD(arg0, s32 *, 0x58);
    M2C_FIELD(arg0, s32 *, 0x68) = var_a2;
    if (t48 >= temp_v1_2) {
        M2C_FIELD(arg0, s32 *, 0x48) = (s32) (temp_v1_2 - 1);
    }
    if ((M2C_FIELD(arg0, s32 *, 0x68) != 0) && (M2C_FIELD(arg0, s32 *, 0x44) == 1) && (M2C_FIELD(arg0, s32 *, 0x48) == (M2C_FIELD(arg0, s32 *, 0x58) - 1))) {
        M2C_FIELD(arg0, s32 *, 0x44) = 0;
    }
    var_a1 = M2C_FIELD(arg0, s32 *, 0x6C);
    temp_v1_3 = M2C_FIELD(arg0, s32 *, 0x58);
    temp_a2 = M2C_FIELD(arg0, s32 *, 0x38);
    if (temp_v1_3 < var_a1) {
        var_a1 = temp_v1_3;
        temp_v1_3 = *(volatile s32 *)((u8 *)arg0 + 0x58);
    }
    temp_v1_4 = temp_v1_3 - var_a1;
    M2C_FIELD(arg0, s32 *, 0x38) = var_a1;
    if (temp_v1_4 < M2C_FIELD(arg0, s32 *, 0x5C)) {
        M2C_FIELD(arg0, s32 *, 0x5C) = temp_v1_4;
    }
    temp_a1 = M2C_FIELD(arg0, void **, 4);
    if ((temp_a1 != NULL) && (M2C_FIELD(temp_a1, s32 *, 0x20) == 1)) {
        M2C_FIELD(temp_a1, s32 *, 0x38) = (s32) (M2C_FIELD(temp_a1, s32 *, 0x38) + (M2C_FIELD(arg0, s32 *, 0x40) * (M2C_FIELD(arg0, s32 *, 0x38) - temp_a2)));
    }
    temp_a1_2 = M2C_FIELD(arg0, s32 *, 0x80);
    if (temp_a1_2 != 0) {
        t58b = M2C_FIELD(arg0, s32 *, 0x58);
        if (M2C_FIELD(arg0, s32 *, 0x6C) >= t58b) {
            MenuWidget_DestroyPopupNode(temp_a1_2);
        }
    } else {
        t58b = M2C_FIELD(arg0, s32 *, 0x58);
        if (M2C_FIELD(arg0, s32 *, 0x6C) < t58b) {
            Draw_SwapPrimBuffers();
        }
    }
}
