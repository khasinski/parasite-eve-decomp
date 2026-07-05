/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: --use-comm-section -G8 */

int g_DrawBlendColor;

int g_DrawBlendColor;
extern int g_DrawScratchBuffer[];

int Draw_GetBlendColor(void) {
    return g_DrawBlendColor;
}

void Draw_BlendColor(int arg0) {
    int *dst;
    int value;

    g_DrawBlendColor = arg0 & 0xFFFFFF;

    value = (((arg0 >> 16) & 0xFF) + ((arg0 >> 8) & 0xFF)) >> 1;
    dst = g_DrawScratchBuffer;
    if (value >= 0x100) {
        value = 0xFF;
    }

    {
        int half = (((arg0 >> 16) & 0xFF) + (arg0 & 0xFF)) >> 1;
        if (half < 0x100) {
            value |= half << 8;
        } else {
            value |= 0xFF00;
        }
    }
    {
        int half = (((arg0 >> 8) & 0xFF) + (arg0 & 0xFF)) >> 1;
        *dst = value | ((half < 0x100) ? (half << 16) : 0xFF0000);
    }
}
