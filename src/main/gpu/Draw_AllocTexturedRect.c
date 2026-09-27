/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --expand-div */
#include "pe1/draw_state.h"
#include "pe1/menu_inventory.h"
#include "pe1/text.h"

static inline void SetCursor(u32 x, u32 y)
{
    D_8009D124 = x;
    D_8009D128 = y;
}

static inline void PushCursor(void)
{
    int *stack = g_TextCursorStack;
    if (stack < g_TextCursorStackTop) {
        int x = D_8009D124, y = D_8009D128;
        g_TextCursorStack = stack + 2;
        stack[0] = x;
        stack[1] = y;
    } else {
        BoundsCheck_AssertStub(2);
    }
}

static inline void PopCursor(void)
{
    int *stack = g_TextCursorStack;
    if (g_TextCursorStackBottom < stack) {
        g_TextCursorStack = stack - 2;
        SetCursor(stack[-2], stack[-1]);
    } else {
        BoundsCheck_AssertStub(3);
    }
}

static inline void PrintSign(u8 *text)
{
    if (text) {
        PushCursor();
        while (*text != 255) {
            Draw_AllocTexturedQuad(*text++);
        }
        PopCursor();
    }
}

static inline int Digit(int leading, int value)
{
    if (!leading || value != 0) {
        return (u8)(value % 10);
    }
    return 15;
}

/* Print a signed decimal field; glyph 15 is padding, table entry 0x70 the
 * minus sign. The sign occupies one place and restores its own text cursor.
 * Retail callers use bounded widths; powers of ten and negation must fit int.
 */
void Draw_AllocTexturedRect(int value, int width)
{
    int divisor = 1;
    int i;

    for (i = 1; i < width; i++) {
        divisor *= 10;
    }
    if (value < 0) {
        value = -value;
        divisor /= 10;
        width--;
        while (value < divisor) {
            Draw_AllocTexturedQuad(15);
            divisor /= 10;
            width--;
        }
        PrintSign(Str_LookupTable4(0x70));
        SetCursor(D_8009D124 + 9u, D_8009D128);
    }
    for (i = 0; i < width; i++) {
        Draw_AllocTexturedQuad(Digit(i < width - 1, value / divisor));
        divisor /= 10;
    }
}
