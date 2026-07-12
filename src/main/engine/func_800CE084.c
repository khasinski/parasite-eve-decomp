typedef unsigned char u8;

void **FieldEng_GetSlot(void);

extern int D_800E0FFC;

extern int D_800E22B8;
extern int D_800E22BC;
extern int D_800E22C0;
extern short D_800E22B0;
extern short D_800E22B2;
extern short D_800E22B4;
extern u8 D_800E22C8;
extern u8 D_800E22C9;
extern u8 D_800E22CA;
extern u8 D_800E22CD;
extern short D_800E22CE;

int func_800CE084(void) {
    void **slot = FieldEng_GetSlot();
    register void *slotData asm("$3");
    register int value asm("$3");

    slotData = &D_800E0FFC;
    *slot = slotData;

    value = 0x300;
    D_800E22B8 = value;
    D_800E22BC = value;
    D_800E22C0 = value;

    value = 5;
    D_800E22CD = value;

    value = -0x64;
    D_800E22CE = value;

    value = 0x80;
    D_800E22B0 = 0;
    D_800E22B2 = 0;
    D_800E22B4 = 0;
    D_800E22C8 = value;
    D_800E22C9 = value;
    D_800E22CA = value;

    return 0;
}
