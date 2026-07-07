/* CC1_FLAGS: -g3 */
/* MASPSX_FLAGS: -G8 --stack-return-delay */
typedef signed char s8;typedef unsigned char u8;typedef short s16;typedef unsigned short u16;typedef int s32;typedef unsigned int u32;typedef long long s64;
#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"
s32 _SpuIsInAllocateArea_();
extern s32 g_SpuCdAudioMixMode[];
#define g_SpuCdAudioMixMode (g_SpuCdAudioMixMode[0])
extern s32 g_SpuCdAudioMixModePrev[];
#define g_SpuCdAudioMixModePrev (g_SpuCdAudioMixModePrev[0])
extern s32 g_SpuCdInputVolume[];
#define g_SpuCdInputVolume (g_SpuCdInputVolume[0])
extern void * _spu_RXX[];
#define _spu_RXX (_spu_RXX[0])

s32 SPU_ReadRegister(s32 arg0) {
    s32 mode;
    void *regs;
    s32 value;

    mode = arg0;
    switch (mode) {
    case 0:
        regs = _spu_RXX;
        value = M2C_FIELD(regs, u16 *, 0x1AA);
        {
            register s32 zero asm("$0");
            g_SpuCdAudioMixMode = zero;
        }
        goto clear_flag;
    case 1:
        if (g_SpuCdAudioMixModePrev != mode) {
            if (_SpuIsInAllocateArea_(g_SpuCdInputVolume) == 0) {
                goto set_flag;
            }
            regs = _spu_RXX;
            value = M2C_FIELD(regs, u16 *, 0x1AA);
            g_SpuCdAudioMixMode = 0;
            goto clear_flag;
        }
set_flag:
        regs = _spu_RXX;
        value = M2C_FIELD(regs, u16 *, 0x1AA);
        g_SpuCdAudioMixMode = mode;
        value |= 0x80;
        goto store_flag;
    default:
        goto done;
    }

clear_flag:
    value &= 0xFF7F;

store_flag:
    M2C_FIELD(regs, s16 *, 0x1AA) = value;

done:
    return g_SpuCdAudioMixMode;
}
