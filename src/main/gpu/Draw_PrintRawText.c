/* CC1_FLAGS: -g3 -G8 */
/* MASPSX_FLAGS: -G8 */
typedef signed char s8;typedef unsigned char u8;typedef short s16;typedef unsigned short u16;typedef int s32;typedef unsigned int u32;typedef long long s64;
#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"
M2C_UNK BoundsCheck_AssertStub();
M2C_UNK Draw_AllocTexturedQuad();
extern s32 g_TextCursorX;
extern s32 g_TextCursorY;
extern u32 g_TextCursorStackPtr;
extern M2C_UNK g_TextCursorStackBottom[];
#define g_TextCursorStackBottom (g_TextCursorStackBottom[0])
extern M2C_UNK g_TextCursorStackTop[];
#define g_TextCursorStackTop (g_TextCursorStackTop[0])

void Draw_PrintRawText(u8 *arg0) {
    s32 temp_a0;
    s32 temp_v0;
    u32 temp_a1;
    u8 *var_s0;
    u8 var_a0;

    var_s0 = arg0;
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
