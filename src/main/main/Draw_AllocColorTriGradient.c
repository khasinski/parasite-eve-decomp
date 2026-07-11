/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
typedef unsigned char u8;
typedef unsigned int u32;
typedef unsigned short u16;

extern int g_TextCursorX;
extern int g_TextCursorY;
extern u16 g_TextCursorStackTop[];
extern u16 *g_DrawVertexWritePtr;
extern u8 D_800930A8[];

extern int D_8009D130;
extern u32 D_8009D14C;
extern u32 D_8009D150;
extern u32 D_800C0E44[];

void BoundsCheck_AssertStub(int arg0);
void Draw_EmitWipeBar(u8 *arg0, int arg1);
void Draw_AllocColorTri(int width, int height, int pulse);

#define PUSH_WIPE_BAR_VERTEX(x, y)                         \
    do {                                                   \
        u16 *out = g_DrawVertexWritePtr;                   \
                                                           \
        if ((unsigned int)out <                            \
            (unsigned int)(g_TextCursorStackTop + 0x18)) { \
            out[1] = (x);                                  \
            g_DrawVertexWritePtr = out + 2;                \
            out[0] = (y);                                  \
        } else {                                           \
            BoundsCheck_AssertStub(4);                     \
        }                                                  \
    } while (0)

#define SET_AVERAGED_RGB(out, color)                            \
    do {                                                        \
        u32 value = (color);                                    \
        int r = (value >> 16) & 0xFF;                           \
        int g = (value >> 8) & 0xFF;                            \
        int b = value & 0xFF;                                   \
        int rg = (r + g) >> 1;                                  \
        int rb;                                                 \
        int gb;                                                 \
                                                                \
        D_8009D14C = value & 0xFFFFFF;                          \
        if (rg >= 0x100) {                                      \
            rg = 0xFF;                                          \
        }                                                       \
                                                                \
        rb = (r + b) >> 1;                                      \
        if (rb >= 0x100) {                                      \
            rb = 0xFF;                                          \
        }                                                       \
                                                                \
        gb = (g + b) >> 1;                                      \
        if (gb >= 0x100) {                                      \
            gb = 0xFF;                                          \
        }                                                       \
                                                                \
        (out) = rg | (rb << 8) | (gb << 16);                    \
    } while (0)

void Draw_AllocColorTriGradient(int width, int height, int mode, int pulse) {
    u32 color;

    color = D_8009D130;
    if (color != 0) {
        SET_AVERAGED_RGB(D_8009D150, color);
    }

    g_DrawVertexWritePtr = g_TextCursorStackTop;
    PUSH_WIPE_BAR_VERTEX(g_TextCursorX, g_TextCursorY);
    PUSH_WIPE_BAR_VERTEX(g_TextCursorX + width, g_TextCursorY);
    PUSH_WIPE_BAR_VERTEX(g_TextCursorX, g_TextCursorY + height);
    PUSH_WIPE_BAR_VERTEX(g_TextCursorX + width, g_TextCursorY + height);

    Draw_EmitWipeBar(D_800930A8, mode);

    g_TextCursorX -= 2;
    g_TextCursorY -= 2;
    Draw_AllocColorTri(width + 4, height + 4, pulse);

    if (D_8009D130 != 0) {
        color = D_800C0E44[0];
        SET_AVERAGED_RGB(D_8009D150, color);
    }
}
