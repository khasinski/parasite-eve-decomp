/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
typedef signed char s8;typedef unsigned char u8;typedef short s16;typedef unsigned short u16;typedef int s32;typedef unsigned int u32;typedef long long s64;
#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"
extern s32 g_SceneDataTable0;
extern void * g_CurrentEntity[];
#define g_CurrentEntity (g_CurrentEntity[0])
extern void *g_TaskNodePool;

s32 Camera_StepAngleFade(u8 *arg0) {
    register s32 temp_a1 asm("$5");
    s32 ta1c;
    register s32 ta1b asm("$6");
    s32 temp_t1;
    u8 *p300;
    register s32 temp_v1 asm("$8");
    s32 tv1a;
    u16 temp_a3;
    register s32 tv0c asm("$8");
    register s32 tv0r asm("$2");
    register s32 ta12c asm("$7");
    register s32 ta1d asm("$4");
    s32 temp_a1_2;
    s32 sv0;
    register s32 temp_v1x asm("$3");
    s32 var_v1;
    register u8 *p13 asm("$2");
    register s32 ret asm("$2");
    s32 tv0;
    register u32 t3araw asm("$2");
    s32 *tv0p;
    register u16 *pa3 asm("$3");
    s32 tor;
    register s32 tdiff asm("$2");
    s32 vext;
    s32 vext2;
    s32 sa3a;
    s32 sa3b;
    s32 sa3c;
    register s32 sv1f asm("$2");
    s32 temp_v0_2;
    s32 tve;
    u8 *p24;
    u8 *p26;
    s32 t3aa;
    register s32 sumb asm("$2");
    register s32 tmask4 asm("$3");
    register s32 ta12d asm("$7");
    s32 sa12a;
    s32 sa12b;
    s32 temp_v1_2;
    s32 temp_v0_3;
    s32 temp_v0_4;

    t3araw = M2C_FIELD(g_CurrentEntity, u16 *, 0x3A);
    temp_a1 = t3araw & 0xFFF;
    p300 = g_TaskNodePool;
    temp_t1 = M2C_FIELD(p300, u16 *, 8);
    ta1c = temp_a1;
    if (!(temp_t1 & 0x20)) {
        ta1b = temp_a1;
        __asm__("" : "=r"(ta1b) : "0"(ta1b));
        tv0p = M2C_FIELD(arg0, s32 **, 0);
        pa3 = M2C_FIELD(arg0, u16 **, 4);
        tv0 = *tv0p;
        temp_a3 = *pa3;
        temp_v1x = tv0 & 0xFFF;
        tv1a = temp_v1x;
        __asm__("" : "=r"(tv1a) : "0"(tv1a));
        temp_v1 = temp_v1x;
        if (ta1b == tv1a) {
            ret = 1;
            goto exit;
        }
        tor = temp_t1 | 0x20;
        M2C_FIELD(p300, u16 *, 8) = (u16) tor;
        __asm__ __volatile__("");
        tdiff = tv1a - ta1b;
        if (tdiff >= 0) {
            var_v1 = temp_v1x - temp_a1;
        } else {
            var_v1 = temp_a1 - temp_v1x;
        }
        if ((s16) temp_v1 < ta1c) {
            vext = var_v1 << 0x10;
            if ((vext >> 0x10) >= 0x801) {
                sa3a = (s16) temp_a3;
                M2C_FIELD(g_TaskNodePool, s32 *, 0x14) = sa3a;
            } else {
                goto block_11;
            }
        } else if ((vext2 = var_v1 << 0x10), ((vext2 >> 0x10) < 0x801)) {
            sa3b = (s16) temp_a3;
            M2C_FIELD(g_TaskNodePool, s32 *, 0x14) = sa3b;
        } else {
block_11:
            sa3c = -(s16) temp_a3;
            M2C_FIELD(g_TaskNodePool, s32 *, 0x14) = sa3c;
        }
        sv1f = (s16) temp_v1;
        M2C_FIELD(g_TaskNodePool, s32 *, 0x18) = sv1f;
    }
    p13 = g_TaskNodePool;
    ta1d = ta1c;
    temp_a1_2 = M2C_FIELD(p13, s32 *, 0x14);
    tv0r = M2C_FIELD(p13, s32 *, 0x18);
    __asm__("" : "=r"(tv0r) : "0"(tv0r));
    tv0c = tv0r;
    __asm__("" : "=r"(tv0c) : "0"(tv0c));
    sv0 = (s16) tv0r;
    ta12c = temp_a1_2;
    if (ta1d < sv0) goto chk2;
    sa12a = (s16) temp_a1_2;
    if (sv0 < (ta1d + sa12a)) goto chk2;
    goto block_24;
chk2:
    ta12d = ta12c;
    __asm__("" : "=r"(ta12d) : "0"(ta12d));
    if (sv0 < ta1d) {
        tve = ta12c << 0x10;
        goto inner;
    }
    sa12b = temp_a1_2 << 0x10;
    sumb = ta1d + (sa12b >> 0x10);
    __asm__("" : : "r"(temp_a1_2));
    if (sumb < sv0) {
        tve = ta12d << 0x10;
        goto inner;
    }
    goto block_24;
inner:
    {
        temp_v0_2 = tve >> 0x10;
        temp_v1_2 = ta1c + temp_v0_2;
        if (temp_v0_2 > 0) {
            if (temp_v1_2 >= 0x1001) {
                if ((temp_v1_2 & 0xFFF) < (s16) tv0c) {
                    goto block_21;
                }
                goto block_24;
            }
            goto block_21;
        }
block_21:
        temp_v0_3 = (s16) ta12c;
        if (temp_v0_3 < 0) {
            temp_v0_4 = ta1c + temp_v0_3;
            if (temp_v0_4 < 0) {
                tmask4 = temp_v0_4 & 0xFFF;
                if ((s16) tv0c < tmask4) {
                    goto block_26;
                }
                goto block_24;
            }
        }
        goto block_26;
    }
block_24:
    p24 = g_TaskNodePool;
    M2C_FIELD(g_CurrentEntity, u16 *, 0x3A) = (u16) tv0c;
    M2C_FIELD(p24, u16 *, 8) = (u16) (M2C_FIELD(p24, u16 *, 8) & 0xFFDF);
    ret = 1;
    goto exit;
block_26:
    ret = 0;
    __asm__ __volatile__("");
    p26 = g_CurrentEntity;
    t3aa = M2C_FIELD(p26, u16 *, 0x3A);
    g_SceneDataTable0 -= 0x10;
    t3aa = t3aa + ta12c;
    M2C_FIELD(p26, u16 *, 0x3A) = (u16) t3aa;
    M2C_FIELD(g_TaskNodePool, s32 *, 0x10) = 1;
exit:
    return ret;
}
