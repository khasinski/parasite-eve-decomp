/* MASPSX_FLAGS: --stack-return-delay */
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
extern M2C_UNK D_80011954[];
#define D_80011954 (D_80011954[0])
extern GpuCallbacks * g_GpuCallbacks[];
#define g_GpuCallbacks (g_GpuCallbacks[0])
extern M2C_UNK (*g_GpuDebugPrintf[])(M2C_UNK *, s32, void *);
#define g_GpuDebugPrintf (g_GpuDebugPrintf[0])
extern u8 g_GraphDebug[];
#define g_GraphDebug (g_GraphDebug[0])

void Gpu_PutDrawEnvLinked(s32 arg0, void *arg1) {
    void *temp_s0;
    register GpuCallbacks *callbacks asm("$3");
    s32 mask;
    register void *packet asm("$5");
    register s32 packetSize asm("$6");
    register s32 highMask asm("$3");
    s32 code;

    if ((u8) g_GraphDebug >= 2U) {
        g_GpuDebugPrintf(&D_80011954, arg0, arg1);
    }
    temp_s0 = arg1 + 0x1C;
    Gpu_SetDrawEnvBack(temp_s0, arg1);
    mask = 0xFFFFFF;
    packet = temp_s0;
    packetSize = 0x40;
    asm volatile("" : : "r"(packetSize));
    highMask = 0xFF000000;
    code = M2C_FIELD(arg1, s32 *, 0x1C);
    mask = arg0 & mask;
    code &= highMask;
    callbacks = g_GpuCallbacks;
    code |= mask;
    M2C_FIELD(arg1, s32 *, 0x1C) = code;
    callbacks->drawEnv(callbacks->drawEnvArg, packet, packetSize, 0);
    call_memcpy(&g_GraphDebug + 0xE, arg1, 0x5C);
}
