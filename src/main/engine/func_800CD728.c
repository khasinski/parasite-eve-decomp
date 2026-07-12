typedef unsigned char u8;
typedef unsigned short u16;

void **FieldEng_GetSlot(void);

extern int D_800E0F6C;

extern int D_800F33F8;
extern int D_800F33FC;
extern int D_800F3400;
extern u8 D_800F3408;
extern u8 D_800F3409;
extern u8 D_800F340A;
extern u8 D_800F340C;
extern u8 D_800F340D;
extern short D_800F340E;
extern short D_800F33F0;
extern short D_800F33F2;
extern short D_800F33F4;

extern int D_800E2780;
extern int D_800E2784;
extern int D_800E2788;
extern u8 D_800E2790;
extern u8 D_800E2791;
extern u8 D_800E2792;
extern u8 D_800E2794;
extern u8 D_800E2795;
extern short D_800E2796;
extern short D_800E2778;
extern short D_800E277A;
extern short D_800E277C;

extern u8 D_800E27D0;
extern u8 D_800E27D1;
extern u8 D_800E27D2;
extern u8 D_800E27D4;
extern u8 D_800E27D5;
extern short D_800E27D6;
extern short D_800E27B8;
extern short D_800E27BA;
extern short D_800E27BC;

int func_800CD728(void) {
    void **slot = FieldEng_GetSlot();
    register int value asm("$3");
    register int byte2 asm("$4");

    value = (int)&D_800E0F6C;
    *slot = (void *)value;

    value = 0x5F4;
    D_800F33F8 = value;
    D_800F33FC = value;
    D_800F3400 = value;

    value = 0x40;
    byte2 = 0x20;
    asm volatile("" : "=r"(byte2) : "0"(byte2));
    D_800F340C = value;
    value = -0x64;
    D_800F340E = value;

    value = 0x15;
    D_800F3408 = value;

    value = 0x46;
    D_800E27D4 = value;

    value = 0x30;
    D_800E27D5 = value;

    value = -0x6E;
    D_800E27D6 = value;

    value = 0xFF;
    D_800E27D0 = value;

    value = 0xB0;
    D_800E27D1 = value;
    D_800E27D2 = value;

    value = 0xA4;
    D_800E2780 = value;
    D_800E2784 = value;

    value = 0x108;
    D_800E2788 = value;

    value = 0x6E;
    D_800E2794 = value;

    value = 3;
    D_800E2795 = value;

    value = -0x96;
    D_800E2796 = value;

    value = 0x80;
    D_800F33F0 = 0;
    D_800F33F2 = 0;
    D_800F33F4 = 0;
    D_800F340D = byte2;
    D_800F3409 = byte2;
    D_800F340A = byte2;
    D_800E27B8 = 0;
    D_800E27BA = 0;
    D_800E27BC = 0;
    D_800E2778 = 0;
    D_800E277A = 0;
    D_800E277C = 0;
    D_800E2790 = value;
    D_800E2791 = value;
    D_800E2792 = value;

    return 0;
}
