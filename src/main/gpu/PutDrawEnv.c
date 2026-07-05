/* CC1_FLAGS: -g3 -G8 */
/* MASPSX_FLAGS: -G8 --stack-return-delay */

#include "include_asm.h"
typedef signed char s8;typedef unsigned char u8;typedef short s16;typedef unsigned short u16;typedef int s32;typedef unsigned int u32;typedef long long s64;
#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"
M2C_UNK Gpu_SetDrawEnvBack();
M2C_UNK call_memcpy(void *, void *, s32) __asm__("memcpy");
typedef struct GpuCallbacks {
    char pad0[8];
    M2C_UNK (*drawEnv)(s32, void *, M2C_UNK, M2C_UNK);
    char padC[0xC];
    s32 drawEnvArg;
} GpuCallbacks;
extern struct { char _[16]; } D_8001193C_o __asm__("D_8001193C");
#define D_8001193C (*(M2C_UNK *)&D_8001193C_o)
extern struct { char _[16]; } g_GpuCallbacks_o __asm__("D_80095744");
#define g_GpuCallbacks (*(GpuCallbacks **)&g_GpuCallbacks_o)
extern struct { char _[16]; } D_80095748_o __asm__("g_GpuDebugPrintf");
#define g_GpuDebugPrintf (*(M2C_UNK (**)(M2C_UNK *, void *))&D_80095748_o)
extern struct { char _[16]; } g_GraphDebug_o __asm__("g_GraphDebug");
#define g_GraphDebug (*(u8 *)&g_GraphDebug_o)

asm(".globl PutDrawEnv");
asm("PutDrawEnv = PutDrawEnv");

void *PutDrawEnv(void *arg0);

void *PutDrawEnv(void *arg0) {
    void *temp_s0;
    GpuCallbacks *callbacks;
    s32 mask;
    void *packet;
    s32 packetSize;
    s32 code;

    if ((u8) g_GraphDebug >= 2U) {
        g_GpuDebugPrintf(&D_8001193C, arg0);
    }
    temp_s0 = arg0 + 0x1C;
    Gpu_SetDrawEnvBack(temp_s0, arg0);
    mask = 0xFFFFFF;
    packet = temp_s0;
    packetSize = 0x40;
    code = M2C_FIELD(arg0, s32 *, 0x1C);
    callbacks = g_GpuCallbacks;
    code |= mask;
    M2C_FIELD(arg0, s32 *, 0x1C) = code;
    callbacks->drawEnv(callbacks->drawEnvArg, packet, packetSize, 0);
    call_memcpy(&g_GraphDebug + 0xE, arg0, 0x5C);
    return arg0;
}
