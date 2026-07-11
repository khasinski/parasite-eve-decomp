/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

typedef unsigned char u8;
typedef unsigned short u16;

extern int g_TextCursorX;
extern int g_TextCursorY;
extern u16 g_TextCursorStackTop[];
extern u16 *g_DrawVertexWritePtr;
extern u8 D_800930A8[];

void BoundsCheck_AssertStub(int arg0);
void Draw_EmitWipeBar(u8 *arg0, int arg1);

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

void Draw_EmitWipeBarRect(int width, int height, int mode) {
    int x0 = g_TextCursorX;
    int y0 = g_TextCursorY;

    g_DrawVertexWritePtr = g_TextCursorStackTop;

    PUSH_WIPE_BAR_VERTEX(x0, y0);
    PUSH_WIPE_BAR_VERTEX(x0 + width, y0);
    PUSH_WIPE_BAR_VERTEX(x0, y0 + height);
    PUSH_WIPE_BAR_VERTEX(x0 + width, y0 + height);

    Draw_EmitWipeBar(D_800930A8, mode);
}
