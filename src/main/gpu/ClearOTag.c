/* CC1_FLAGS: -g3 -G8 */
/* MASPSX_FLAGS: -G8 --stack-return-delay */

typedef signed char s8;
typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;
typedef int s32;
typedef unsigned int u32;
typedef long long s64;

#include "../../../tools/m2c/m2c_macros.h"

extern M2C_UNK D_800118F8[];
#define D_800118F8 (D_800118F8[0])

extern M2C_UNK (*g_GpuDebugPrintf[])(M2C_UNK *, void *, s32);
#define g_GpuDebugPrintf (g_GpuDebugPrintf[0])

extern u8 g_GraphDebug[];
#define g_GraphDebug (g_GraphDebug[0])

extern M2C_UNK D_800957F8[];
#define D_800957F8 (D_800957F8[0])

extern s32 D_8009580C[];
#define D_8009580C (D_8009580C[0])

asm(".globl func_800751E4");
asm("func_800751E4 = ClearOTag");

void ClearOTag(void *arg0, s32 arg1) {
    u8 *next;
    u8 *packet;
    register s32 mask24 asm("$5");
    s32 maskTop;
    s32 temp_v1;
    register s32 temp_a0 asm("$4");
    register M2C_UNK (*callback)(M2C_UNK *, void *, s32) asm("$2");
    register u8 *tail asm("$2");

    packet = arg0;
    {
        register s32 count asm("$17");

        count = arg1;
        if ((u8)g_GraphDebug >= 2U) {
            callback = g_GpuDebugPrintf;
            callback(&D_800118F8, packet, count);
        }

        count -= 1;
        if (count != 0) {
            mask24 = 0xFFFFFF;
            maskTop = 0xFF000000;
            do {
                count -= 1;
                next = packet + 4;
                M2C_FIELD(packet, s8 *, 3) = 0;
                M2C_FIELD(packet, s32 *, 0) = (M2C_FIELD(packet, s32 *, 0) & maskTop) | ((s32)next & mask24);
                packet = next;
            } while (count != 0);
        }

        maskTop = 0xFFFFFF;
        tail = packet;
        __asm__ volatile("" : "=r"(tail) : "0"(tail));
        mask24 = (s32)&D_8009580C;
        temp_v1 = (s32)&D_800957F8;
        temp_v1 &= maskTop;
        temp_a0 = 0x04000000;
        temp_v1 |= temp_a0;
        M2C_FIELD((void *)mask24, s32 *, 0) = temp_v1;
        mask24 &= maskTop;
        M2C_FIELD(tail, s32 *, 0) = mask24;
    }
}
