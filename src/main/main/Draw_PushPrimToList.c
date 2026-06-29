/* CC1_FLAGS: -g3 -G8 */
/* MASPSX_FLAGS: -G8 */
typedef signed char s8;typedef unsigned char u8;typedef short s16;typedef unsigned short u16;typedef int s32;typedef unsigned int u32;typedef long long s64;
#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"
M2C_UNK BoundsCheck_AssertStub();
extern s32 g_TextCursorX;
extern u32 g_TextCursorY;
extern u32 g_TextCursorStackPtr;
extern struct { char _[16]; } D_800A2270_o __asm__("g_TextCursorStackBottom");
#define g_TextCursorStackBottom (*(M2C_UNK *)&D_800A2270_o)
extern struct { char _[16]; } D_800A22B0_o __asm__("g_TextCursorStackTop");
#define g_TextCursorStackTop (*(M2C_UNK *)&D_800A22B0_o)

void Draw_PushPrimToList(void *arg0) {
    s32 temp_a2;
    s32 temp_v0;
    s32 temp_v0_2;
    s32 var_s1;
    u32 temp_a0;
    u32 temp_a0_3;
    u32 temp_a1;
    u32 temp_a1_2;
    void *temp_a0_2;
    register void *var_s0 asm("$16");
    u8 *q;
    s32 t0;
    u32 t1;
    M2C_UNK (*f)(void *);
    register s32 tf asm("$3");
    register s32 tg asm("$2");

    var_s0 = arg0;
    temp_a1 = g_TextCursorStackPtr;
    if (temp_a1 < (u32) &g_TextCursorStackTop) {
        t0 = (s32) g_TextCursorX;
        t1 = (u32) g_TextCursorY;
        g_TextCursorStackPtr = temp_a1 + 8;
        M2C_FIELD(temp_a1, s32 *, 0) = t0;
        M2C_FIELD(temp_a1, u32 *, 4) = t1;
    } else {
        BoundsCheck_AssertStub(2, temp_a1);
    }
    tf = M2C_FIELD(var_s0, s32 *, 0x18);
    tg = g_TextCursorX;
    f = M2C_FIELD(var_s0, M2C_UNK (**)(void *), 0x30);
    temp_a2 = tg + tf;
    tf = M2C_FIELD(var_s0, s32 *, 0x1C);
    tg = g_TextCursorY;
    g_TextCursorX = temp_a2;
    temp_a1_2 = tg + tf;
    g_TextCursorY = temp_a1_2;
    var_s1 = 0;
    if (f != NULL) {
        q = (u8 *) g_TextCursorStackPtr;
        if ((u32) q < (u32) &g_TextCursorStackTop) {
            *(s32 *) q = temp_a2;
            *(u32 *) (q + 4) = temp_a1_2;
            g_TextCursorStackPtr = (u32) (q + 8);
        } else {
            BoundsCheck_AssertStub(2, temp_a1_2, temp_a2);
        }
        M2C_FIELD(var_s0, M2C_UNK (**)(void *), 0x30)(var_s0);
        __asm__ volatile("");
        var_s1 = 0;
        if ((u32) &g_TextCursorStackBottom < (u32) g_TextCursorStackPtr) {
            temp_v0 = M2C_FIELD(g_TextCursorStackPtr, s32 *, -8);
            temp_a0 = M2C_FIELD(g_TextCursorStackPtr, u32 *, -4);
            g_TextCursorStackPtr -= 8;
            g_TextCursorX = temp_v0;
            g_TextCursorY = temp_a0;
        } else {
            BoundsCheck_AssertStub(3);
            __asm__("" : "=r"(var_s1) : "0"(var_s1));
            var_s1 = 0;
        }
    }
    do {
        temp_a0_2 = M2C_FIELD(var_s0, void **, 8);
        if (temp_a0_2 != NULL) {
            Draw_PushPrimToList(temp_a0_2);
        }
        var_s1 += 1;
        var_s0 += 4;
    } while (var_s1 < 4);
    if ((u32) &g_TextCursorStackBottom < (u32) g_TextCursorStackPtr) {
        temp_v0_2 = M2C_FIELD(g_TextCursorStackPtr, s32 *, -8);
        temp_a0_3 = M2C_FIELD(g_TextCursorStackPtr, u32 *, -4);
        g_TextCursorStackPtr -= 8;
        g_TextCursorX = temp_v0_2;
        g_TextCursorY = temp_a0_3;
        return;
    }
    BoundsCheck_AssertStub(3);
}
