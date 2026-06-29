/* CC1_FLAGS: -g3 -G8 */
/* MASPSX_FLAGS: -G8 */
typedef signed char s8;typedef unsigned char u8;typedef short s16;typedef unsigned short u16;typedef int s32;typedef unsigned int u32;typedef long long s64;
#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"
M2C_UNK BoundsCheck_AssertStub();
u8 *Str_LookupTable4() __asm__("func_8005DC4C");
M2C_UNK Draw_AllocTexturedQuad();
extern s32 g_TextCursorX;
extern s32 g_TextCursorY;
extern u32 g_TextCursorStackPtr;
extern struct { char _[16]; } D_800A2270_o __asm__("g_TextCursorStackBottom");
#define g_TextCursorStackBottom (*(M2C_UNK *)&D_800A2270_o)
extern struct { char _[16]; } D_800A22B0_o __asm__("g_TextCursorStackTop");
#define g_TextCursorStackTop (*(M2C_UNK *)&D_800A22B0_o)

void Draw_PrintTextById(void) {
    s32 temp_a0;
    s32 temp_v0;
    u32 temp_a1;
    u8 *var_s0;
    u8 var_a0;

    var_s0 = Str_LookupTable4();
    if (var_s0 != NULL) {
        temp_a1 = g_TextCursorStackPtr;
        if (temp_a1 < (u32) &g_TextCursorStackTop) {
            s32 t0 = g_TextCursorX; s32 t1 = g_TextCursorY;
            g_TextCursorStackPtr = temp_a1 + 8;
            M2C_FIELD(temp_a1, s32 *, 0) = t0;
            M2C_FIELD(temp_a1, s32 *, 4) = t1;
        } else {
            BoundsCheck_AssertStub(2, temp_a1);
        }
        var_a0 = *var_s0;
        if ((var_a0 & 0xFF) != 0xFF) {
            do {
                var_s0 += 1;
                Draw_AllocTexturedQuad(var_a0);
                var_a0 = *var_s0;
            } while (var_a0 != 0xFF);
        }
        if ((u32) &g_TextCursorStackBottom < (u32) g_TextCursorStackPtr) {
            temp_v0 = M2C_FIELD(g_TextCursorStackPtr, s32 *, -8);
            temp_a0 = M2C_FIELD(g_TextCursorStackPtr, s32 *, -4);
            g_TextCursorStackPtr -= 8;
            g_TextCursorX = temp_v0;
            g_TextCursorY = temp_a0;
            return;
        }
        BoundsCheck_AssertStub(3);
    }
}
