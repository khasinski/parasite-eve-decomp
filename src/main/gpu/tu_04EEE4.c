/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: --use-comm-section -G8 */

int g_DrawPresentImage;

typedef unsigned char u8;
typedef unsigned int u32;

extern int g_ActiveDrawSlot[];
extern int D_8009D0FC;
extern int g_ActiveDrawBuffer;
extern int g_DrawPacketBufferBase;
extern int g_DrawBufferIndex;
extern int D_8009D118;
extern int g_OtListTail;
extern int g_DrawPresentEnabled;
extern u8 D_800A2180[];
extern int g_DrawBufferOtBases[];
extern int g_DrawBufferFrontBases[];

void ClearOTagR(int arg0, int arg1);

void Draw_SetPresentImage(int arg0) {
    g_DrawPresentImage = arg0;
}

void Draw_SelectBuffer(void) {
    int index;
    int offset;
    u8 *entry;
    int value0;
    int value1;

    if (g_DrawPresentEnabled != 0) {
        index = (u32)g_DrawBufferIndex < 1;
    } else {
        index = g_ActiveDrawSlot[0];
    }

    offset = ((index << 4) - index) << 3;
    g_DrawBufferIndex = index;
    entry = D_800A2180 + offset;
    value0 = *(int *)((u8 *)g_DrawBufferFrontBases + offset);
    value1 = *(int *)((u8 *)g_DrawBufferOtBases + offset);
    D_8009D0FC = (int)entry;
    g_DrawPacketBufferBase = value0;
    g_ActiveDrawBuffer = value0;
    D_8009D118 = value1;
    g_OtListTail = value1 + 4;

    if (g_DrawPresentEnabled != 0) {
        ClearOTagR(value1, 0x1000);
    }
}
