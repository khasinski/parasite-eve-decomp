/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

typedef unsigned char u8;
typedef unsigned int u32;
typedef unsigned short u16;

extern int g_TextCursorX;
extern int g_TextCursorY;
extern u16 g_TextCursorStackTop[];
extern u16 *g_DrawVertexWritePtr;

void BoundsCheck_AssertStub(int arg0);
void Draw_EmitWipeBar(u8 *arg0, int arg1);

void Draw_EmitWipeBarPoly(int arg0, int arg1, u8 *arg2) {
    u8 *cursor = arg2;
    u32 value;
    u16 *end;

    g_DrawVertexWritePtr = g_TextCursorStackTop;
    value = cursor[0];
    if (value < 0xFF) {
        end = g_TextCursorStackTop + 0x18;
        do {
            u16 *out = g_DrawVertexWritePtr;
            int x = value + g_TextCursorX;
            int y = g_TextCursorY + cursor[1];

            if ((unsigned int)out < (unsigned int)end) {
                out[1] = x;
                g_DrawVertexWritePtr = out + 2;
                out[0] = y;
            } else {
                BoundsCheck_AssertStub(4);
            }

            cursor += 2;
            value = cursor[0];
        } while (value < 0xFF);
    }

    Draw_EmitWipeBar(cursor + 1, 0);
}
