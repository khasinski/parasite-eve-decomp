/* CC1_FLAGS: -g3 -G8 -fno-schedule-insns */
/* MASPSX_FLAGS: -G8 */

typedef unsigned char u8;
typedef unsigned short u16;
typedef int s32;

extern u8 g_RenderStateFlags[];
extern struct { char _[16]; } D_800BCFE8_o __asm__("D_800BCFE8");
#define D_800BCFE8 (*(u16 *)&D_800BCFE8_o)
extern struct { char _[16]; } D_800BCFEA_o __asm__("g_ScreenTransitionTargetY");
#define g_ScreenTransitionTargetY (*(u16 *)&D_800BCFEA_o)
extern struct { char _[16]; } D_800BCFEC_o __asm__("D_800BCFEC");
#define D_800BCFEC (*(u16 *)&D_800BCFEC_o)
extern struct { char _[16]; } D_800BCFEE_o __asm__("g_ScreenTransitionState");
#define g_ScreenTransitionState (*(u8 *)&D_800BCFEE_o)
extern struct { char _[16]; } D_800BCFEF_o __asm__("g_ScreenTransitionMode");
#define g_ScreenTransitionMode (*(u8 *)&D_800BCFEF_o)
extern struct { char _[16]; } D_800BCFF0_o __asm__("D_800BCFF0");
#define D_800BCFF0 (*(u16 *)&D_800BCFF0_o)
extern struct { char _[16]; } D_800BCFF2_o __asm__("g_ScreenTransitionStartY");
#define g_ScreenTransitionStartY (*(u16 *)&D_800BCFF2_o)
extern struct { char _[16]; } D_800BCFF4_o __asm__("D_800BCFF4");
#define D_800BCFF4 (*(u16 *)&D_800BCFF4_o)

s32 CdRom_InitScreenState(void) {
    s32 i;
    s32 value3;
    s32 value60;
    s32 value140;
    s32 valueE0;
    s32 value1;
    s32 command;
    register u8 *base asm("$6");
    register u8 *p asm("$4");
    register u8 *q asm("$5");
    register s32 tmp2 asm("$2");
    register s32 tmp3 asm("$3");

    i = 0;
    value3 = 3;
    value60 = 0x60;
    value140 = 0x140;
    valueE0 = 0xE0;
    value1 = 1;
    command = 0xE1000400;
    base = g_RenderStateFlags;
    q = base;
    p = base;

    D_800BCFEC = 0xFF;
    g_ScreenTransitionTargetY = 0xFF;
    D_800BCFE8 = 0xFF;
    g_ScreenTransitionState = 1;
    D_800BCFF4 = 0;
    g_ScreenTransitionStartY = 0;
    D_800BCFF0 = 0;
    g_ScreenTransitionMode = 2;

    do {
        p[0x33] = value3;
        p[0x37] = value60;
        tmp2 = *(u16 *)(base + 0x60);
        p[0x34] = tmp2;
        tmp2 = *(u16 *)(base + 0x62);
        p[0x35] = tmp2;
        tmp3 = *(u16 *)(base + 0x64);
        i++;
        *(u16 *)(p + 0x38) = 0;
        *(u16 *)(p + 0x3A) = 0;
        *(u16 *)(p + 0x3C) = value140;
        *(u16 *)(p + 0x3E) = valueE0;
        p[0x37] |= 2;
        p[0x36] = tmp3;
        q[0x53] = value1;
        tmp2 = base[0x67];
        p += 0x10;
        tmp2 &= 3;
        tmp2 <<= 5;
        tmp2 |= command;
        *(s32 *)(q + 0x54) = tmp2;
        q += 8;
    } while (i < 2);

    tmp2 = 0;
    asm volatile("" : : "r"(tmp2));
    *(u16 *)(base + 0x6E) = 0;
    *(u16 *)(base + 0x70) = 0;
    return tmp2;
}
