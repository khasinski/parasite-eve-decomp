/* CC1_FLAGS: -g3 -G8 */
/* MASPSX_FLAGS: -G8 --expand-div */
typedef signed char s8;typedef unsigned char u8;typedef short s16;typedef unsigned short u16;typedef int s32;typedef unsigned int u32;typedef long long s64;
#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"
void MenuWidget_EaseNodePosition(void *arg0) {
    s32 temp_a2;
    s32 temp_v1;
    s32 temp_v1_2;
    void *temp_a1;

    temp_a1 = M2C_FIELD(arg0, void **, 0x34);
    if (arg0 != NULL) {
        temp_v1 = M2C_FIELD(temp_a1, s32 *, 0x38);
        temp_a2 = M2C_FIELD(temp_a1, s32 *, 0x58);
        if (temp_v1 < temp_a2) {
            M2C_FIELD(arg0, s32 *, 0x3C) = (s32) ((s32) (M2C_FIELD(temp_a1, s32 *, 0x40) * temp_v1 * temp_v1) / temp_a2);
            temp_v1_2 = M2C_FIELD(temp_a1, s32 *, 0x40);
            M2C_FIELD(arg0, s32 *, 0x38) = (s32) ((s32) (temp_v1_2 * M2C_FIELD(temp_a1, s32 *, 0x38) * ((temp_v1_2 * M2C_FIELD(temp_a1, s32 *, 0x5C)) - M2C_FIELD(temp_a1, s32 *, 0x60))) / (s32) (M2C_FIELD(temp_a1, s32 *, 0x58) * temp_v1_2));
        }
    }
}
