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
    int value;
    int half;
    int byte;
    int byte2;

    *slot = &D_800E0F6C;

    value = 0x5F4;
    D_800F33F8 = value;
    D_800F33FC = value;
    D_800F3400 = value;

    byte = 0x40;
    half = -0x64;
    byte2 = 0x20;
    D_800F340C = byte;
    D_800F340E = half;

    byte = 0x15;
    D_800F3408 = byte;

    byte = 0x46;
    D_800E27D4 = byte;

    byte = 0x30;
    D_800E27D5 = byte;

    half = -0x6E;
    D_800E27D6 = half;

    byte = 0xFF;
    D_800E27D0 = byte;

    byte = 0xB0;
    D_800E27D1 = byte;
    D_800E27D2 = byte;

    value = 0xA4;
    D_800E2780 = value;
    D_800E2784 = value;

    value = 0x108;
    D_800E2788 = value;

    byte = 0x6E;
    D_800E2794 = byte;

    byte = 3;
    D_800E2795 = byte;

    half = -0x96;
    D_800E2796 = half;

    byte = 0x80;
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
    D_800E2790 = byte;
    D_800E2791 = byte;
    D_800E2792 = byte;

    return 0;
}
