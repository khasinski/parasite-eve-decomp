typedef unsigned char u8;
typedef unsigned short u16;

extern u16 g_RenderDrawBaseY1;
extern u16 g_RenderDrawBaseY0;
extern u8 g_DrawBaseY;

int Draw_SetBaseY(int arg0, unsigned int arg1) {
    register int value asm("$2");

    if (arg1 < 0x10) {
        value = arg1;
        g_RenderDrawBaseY1 = value;
        g_RenderDrawBaseY0 = value;
        g_DrawBaseY = value;
    }

    return 0;
}
