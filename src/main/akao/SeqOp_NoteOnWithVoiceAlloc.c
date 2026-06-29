/* CC1_FLAGS: -g3 -G8 */
/* MASPSX_FLAGS: -G8 */
typedef signed char s8;
typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;
typedef int s32;
typedef unsigned int u32;
typedef long long s64;

#define NULL ((void *)0)

#include "../../../tools/m2c/m2c_macros.h"

M2C_UNK SeqOp_SetTrack38Mask();
extern struct { char _[16]; } g_AkaoCurTrack_o __asm__("g_AkaoCurTrack");
#define g_AkaoCurTrack (*(void **)&g_AkaoCurTrack_o)

void SeqOp_NoteOnWithVoiceAlloc(void *arg0, s32 arg1, s32 arg2, s16 arg3) {
    register s16 var_a3 asm("$7");
    register s32 var_a2 asm("$6");
    s32 temp_v1_2;
    register s32 temp_t0 asm("$8");
    s32 temp_flags;
    u8 *temp_v1;

    var_a2 = arg2;
    var_a3 = arg3;
    temp_v1 = M2C_FIELD(arg0, u8 **, 0);
    M2C_FIELD(arg0, u8 **, 0) = (u8 *) (temp_v1 + 1);
    temp_flags = M2C_FIELD(arg0, s32 *, 0x38);
    M2C_FIELD(arg0, s16 *, 0x5E) = (s16) (*temp_v1 << 8);
    M2C_FIELD(arg0, s16 *, 0x60) = 0;
    if (!(temp_flags & 0x800)) {
        var_a3 = 0;
        var_a2 = 1;
        temp_t0 = 0xFFFFFF;
        temp_v1_2 = M2C_FIELD(g_AkaoCurTrack, s32 *, 4) | M2C_FIELD(g_AkaoCurTrack, s32 *, 0x30);
loop_2:
        if (temp_v1_2 & var_a2) {
            var_a2 *= 2;
            var_a3 += 1;
            if (!(var_a2 & temp_t0)) {

            } else {
                goto loop_2;
            }
        }
        if (var_a2 & 0xFFFFFF) {
            M2C_FIELD(g_AkaoCurTrack, s32 *, 0x30) = (s32) (M2C_FIELD(g_AkaoCurTrack, s32 *, 0x30) | var_a2);
            M2C_FIELD(arg0, s16 *, 0x5C) = var_a3;
            M2C_FIELD(arg0, s32 *, 0x38) = (s32) (M2C_FIELD(arg0, s32 *, 0x38) | 0x800);
        }
    }
    SeqOp_SetTrack38Mask();
}
