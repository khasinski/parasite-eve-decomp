/* CC1_FLAGS: -g3 -G8 */
typedef signed char s8;typedef unsigned char u8;typedef short s16;typedef unsigned short u16;typedef int s32;typedef unsigned int u32;typedef long long s64;
#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"

extern struct { char _[16]; } D_8009CDDC_o __asm__("g_ActiveDrawSlot");
#define g_ActiveDrawSlot (*(s32 *)&D_8009CDDC_o)
extern struct { char _[32]; } D_800921D8_o __asm__("D_800921D8");

void Field_GetMapEntry(void *arg0, s32 arg1) {
    void *t0;
    register s32 t1 asm("$9");
    s32 a3;
    u8 *a2;
    s32 a0;
    s32 a1;
    register s32 v0 asm("$2");
    register void *v1 asm("$3");

    __asm__ volatile(
        "addiu $sp,$sp,-0x10\n\t"
        "addu %0,%1,$zero"
        : "=r"(t0)
        : "r"(arg0)
        : "memory");
    v1 = M2C_FIELD(t0, void **, 0);
    a3 = g_ActiveDrawSlot;
    a0 = 0;
    if (v1 != NULL) {
        if (M2C_FIELD(t0, s16 *, 0xBA) != 0) {
            v0 = M2C_FIELD(v1, u16 *, 8);
            a2 = M2C_FIELD(t0, u8 **, 0x54);
            if (v0 != 0) {
                v0 = ((((a3 << 1) + a3) << 2) + a3) << 2;
                goto has_entry;
            } else {
                v0 = M2C_FIELD(v1, u16 *, 0xA);
                if (v0 != 0) {
                    v0 = ((a3 << 2) + a3) << 3;
has_entry:
                    v0 = (s32) ((u8 *) v0 + (s32) a2);
                    v0 = M2C_FIELD((void *) v0, u16 *, 0x1A);
                    v0 &= 0x7F;
                    a0 = (u32) v0 >> 5;
                }
            }
            v0 = arg1 << 16;
            t1 = v0 >> 16;
            if (t1 != a0) {
                v0 = a0 << 3;
                v1 = &D_800921D8_o;
                v0 = (s32) ((u8 *)v1 + v0);
                v1 = (void *)(t1 << 1);
                a0 = M2C_FIELD((u8 *)v1 + v0, s16 *, 0);
                v0 = M2C_FIELD(M2C_FIELD(t0, void **, 0), u16 *, 8);
                a1 = 0;
                if (v0 > 0) {
                    v0 = ((((a3 << 1) + a3) << 2) + a3);
                    t1 = v0 << 2;
                    do {
                        v1 = a2 + t1;
                        M2C_FIELD(v1, u16 *, 0x1A) = M2C_FIELD(v1, u16 *, 0x1A) + a0;
                        v0 = M2C_FIELD(M2C_FIELD(t0, void **, 0), u16 *, 8);
                        __asm__ volatile("" ::: "memory");
                        a1 += 1;
                        a2 += 0x68;
                    } while (a1 < v0);
                }
                v0 = M2C_FIELD(M2C_FIELD(t0, void **, 0), u16 *, 0xA);
                a1 = 0;
                if (v0 > 0) {
                    v0 = (a3 << 2) + a3;
                    a3 = v0 << 3;
                    do {
                        v1 = a2 + a3;
                        M2C_FIELD(v1, u16 *, 0x1A) = M2C_FIELD(v1, u16 *, 0x1A) + a0;
                        v0 = M2C_FIELD(M2C_FIELD(t0, void **, 0), u16 *, 0xA);
                        __asm__ volatile("" ::: "memory");
                        a1 += 1;
                        a2 += 0x50;
                    } while (a1 < v0);
                }
            }
        }
    }
    __asm__ volatile("addiu $sp,$sp,0x10");
}
