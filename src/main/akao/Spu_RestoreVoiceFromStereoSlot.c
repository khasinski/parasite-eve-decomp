/* CC1_FLAGS: -g3 -G8 */
/* MASPSX_FLAGS: -G8 */
typedef signed char s8;typedef unsigned char u8;typedef short s16;typedef unsigned short u16;typedef int s32;typedef unsigned int u32;typedef long long s64;
#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"

M2C_UNK Akao_WriteVoiceParam();

extern struct { char _[16]; } D_800B8AC0_o __asm__("g_AkaoVoiceStateTable");
#define g_AkaoVoiceStateTable (*(M2C_UNK *)&D_800B8AC0_o)

void Spu_RestoreVoiceFromStereoSlot(void *arg0, s32 arg1) {
    void *arg0_reg;
    s32 arg1_reg;
    s32 temp_s2;
    register s32 mask asm("$7");
    register s32 idx_arg asm("$4");
    s32 arg_a2;
    s32 call_a1;
    s32 index;
    register u32 val118 asm("$3");
    register u32 val11A asm("$2");
    register u32 flags asm("$3");
    void *temp_v0;

    arg0_reg = arg0;
    arg1_reg = arg1;
    temp_s2 = arg0_reg + 0xF0;
    Akao_WriteVoiceParam(M2C_FIELD(arg0_reg, s32 *, 0xF0), temp_s2, M2C_FIELD(arg0_reg, s32 *, 0x38));
    mask = 0x1FF93;
    idx_arg = arg1_reg;
    asm volatile("" : "=r"(idx_arg) : "0"(idx_arg));
    index = idx_arg;
    index = index << 3;
    index += idx_arg;
    index = index << 3;
    index -= idx_arg;
    index = index << 2;
    temp_v0 = (void *) ((s32) &g_AkaoVoiceStateTable + index);
    arg_a2 = M2C_FIELD(arg0_reg, s32 *, 0x38);
    val118 = M2C_FIELD(temp_v0, u16 *, 0x118);
    call_a1 = temp_s2;
    M2C_FIELD(arg0_reg, u16 *, 0x118) = (u16) val118;
    flags = M2C_FIELD(arg0_reg, s32 *, 0xF4);
    val11A = M2C_FIELD(temp_v0, u16 *, 0x11A);
    flags = flags | mask;
    M2C_FIELD(arg0_reg, s32 *, 0xF4) = flags;
    M2C_FIELD(arg0_reg, u16 *, 0x11A) = (u16) val11A;
    Akao_WriteVoiceParam(idx_arg, call_a1, arg_a2, mask);
}
