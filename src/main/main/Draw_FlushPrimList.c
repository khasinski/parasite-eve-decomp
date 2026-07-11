/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
typedef unsigned char u8;
typedef unsigned int u32;

extern int g_TextCursorX;
extern int g_TextCursorY;
extern int *g_TextCursorStackPtr;
extern int g_TextCursorStackBottom[];
extern int g_TextCursorStackTop[];
extern int D_8009CDB0;
extern int D_8009D0D8;
extern int D_8009D138;

u8 *Str_LookupTable4(void);
int Draw_LookupGlyphMetrics(int glyph);
void Draw_AllocTexturedQuad(int glyph);
void BoundsCheck_AssertStub(int arg0);

#define PUSH_CURSOR()                                      \
    do {                                                   \
        int *cursor = g_TextCursorStackPtr;                \
                                                           \
        if ((u32)cursor < (u32)g_TextCursorStackTop) {     \
            g_TextCursorStackPtr = cursor + 2;             \
            cursor[0] = g_TextCursorX;                     \
            cursor[1] = g_TextCursorY;                     \
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

void Draw_FlushPrimList(void) {
    u8 *text;
    u8 *cursor;
    int width;
    int glyph;
    int ch;

    text = Str_LookupTable4();

    PUSH_CURSOR();

    cursor = text;
    width = 0;
    if (*cursor != 0xFF) {
        do {
            ch = *cursor++;
            glyph = ch;
            if (D_8009D0D8 != 0) {
                glyph += D_8009D0D8 << 8;
                D_8009D0D8 = 0;
            }

            ch &= 0xFF;
            if ((u32)ch >= 0xFA) {
                D_8009D0D8 = ch - 0xFA;
                glyph = -1;
            }

            if (glyph >= 0) {
                D_8009CDB0 = ((glyph < 10) || (glyph == 15)) + 1;
                if (glyph >= 0x100) {
                    glyph -= 0x13;
                }
                width += ((Draw_LookupGlyphMetrics(glyph) >> 4) & 0xF) + D_8009CDB0;
            }
        } while (*cursor != 0xFF);
        cursor = text;
    }

    g_TextCursorX += (D_8009D138 - (width + 4)) >> 1;
    g_TextCursorY = g_TextCursorY;

    if (cursor != 0) {
        PUSH_CURSOR();

        ch = *cursor;
        if ((ch & 0xFF) != 0xFF) {
            do {
                Draw_AllocTexturedQuad(ch);
                cursor++;
                ch = *cursor;
            } while (ch != 0xFF);
        }

        POP_CURSOR();
    }

    POP_CURSOR();
}
