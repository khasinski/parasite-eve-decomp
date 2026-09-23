#include "common.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

extern int g_TextCursorX;
extern int g_TextCursorY;
extern int *g_TextCursorStackPtr;
extern int g_TextCursorStackBottom[];
extern int g_TextCursorStackTop[];
extern int D_8009CDB0;
extern int D_8009D0D8;
extern int D_8009D138;

u8 *Str_LookupTable4(int textId);
int Draw_LookupGlyphMetrics(int glyph);
void Draw_AllocTexturedQuad(int glyph);
void BoundsCheck_AssertStub(int arg0);

#define PUSH_CURSOR()                                                      \
    do {                                                                   \
        int *cursor = g_TextCursorStackPtr;                                \
        register int *next asm("$3") = cursor + 2;                          \
        if ((u32)cursor < (u32)g_TextCursorStackTop) {                     \
            int x = g_TextCursorX;                                         \
            int y = g_TextCursorY;                                         \
            g_TextCursorStackPtr = next;                                   \
            cursor[0] = x;                                                 \
            cursor[1] = y;                                                 \
        } else {                                                           \
            BoundsCheck_AssertStub(2);                                     \
        }                                                                  \
    } while (0)

#define PUSH_CURSOR2()                                     \
    do {                                                   \
        int *cursor = g_TextCursorStackPtr;                \
        if ((u32)cursor < (u32)g_TextCursorStackTop) {     \
            int *next = cursor + 2;                        \
            int x = g_TextCursorX;                         \
            int y = g_TextCursorY;                         \
            cursor[0] = x;                                 \
            cursor[1] = y;                                 \
            g_TextCursorStackPtr = next;                   \
        } else {                                           \
            BoundsCheck_AssertStub(2);                     \
        }                                                  \
    } while (0)

#define POP_CURSOR()                                       \
    do {                                                   \
        int *cursor = g_TextCursorStackPtr;                \
                                                           \
        if ((u32)g_TextCursorStackBottom < (u32)cursor) {  \
            g_TextCursorX = cursor[-2];                    \
            g_TextCursorY = cursor[-1];                    \
            g_TextCursorStackPtr = cursor - 2;             \
        } else {                                           \
            BoundsCheck_AssertStub(3);                     \
        }                                                  \
    } while (0)

void Draw_FlushPrimList(int textId) {
    u8 *text;
    u8 *cursor;
    int width;
    int glyph;
    int ch;
    int first;

    text = Str_LookupTable4(textId);

    PUSH_CURSOR();

    first = *text;
    cursor = text;
    width = 0;
    if (first != 0xFF) {
        do {
            ch = *cursor++;
            glyph = ch & 0xFF;
            if (D_8009D0D8 != 0) {
                glyph += D_8009D0D8 << 8;
                D_8009D0D8 = 0;
            }

            ch &= 0xFF;
            if ((u32)ch >= 0xFA) {
                D_8009D0D8 = ch - 0xFA;
                glyph = -1;
            }

            ch = glyph;
            if (ch >= 0) {
                D_8009CDB0 = ((ch < 10) || (ch == 15)) + 1;
                if (ch >= 0x100) {
                    ch -= 0x13;
                }
                width += ((Draw_LookupGlyphMetrics(ch) >> 4) & 0xF) + D_8009CDB0;
            }
        } while (*cursor != 0xFF);
        cursor = text;
    }

    {
        int adjusted = width + 4;
        int center = D_8009D138 - adjusted;
        int y = g_TextCursorY;
        int x = g_TextCursorX;
        asm volatile("" : : "r"(x));
        center >>= 1;
        asm volatile("" : "=r"(y) : "0"(y));
        g_TextCursorY = y;
        asm volatile("" : : "m"(g_TextCursorY));
        g_TextCursorX = x + center;
    }

    if (cursor != 0) {
        PUSH_CURSOR2();

        ch = *cursor;
        {
            int masked;
            register int end asm("$2") = 0xFF;
            asm volatile("" : "=r"(ch) : "0"(ch));
            masked = ch & 0xFF;
            if (masked != end) {
                do {
                    Draw_AllocTexturedQuad(ch);
                    cursor++;
                    ch = *cursor;
                } while (ch != 0xFF);
            }
        }

        POP_CURSOR();
    }

    POP_CURSOR();
}
