typedef unsigned char u8;
typedef signed short s16;

void **FieldEng_GetSlot(void);

extern int D_800E0B38;

extern u8 D_800E22F8;
extern u8 D_800E22F9;
extern u8 D_800E22FA;
extern u8 D_800E22FC;
extern u8 D_800E22FD;
extern u8 D_800E22FE;
extern s16 D_800E2300;
extern s16 D_800E2302;

extern u8 D_800F34B8;
extern u8 D_800F34B9;
extern u8 D_800F34BA;
extern u8 D_800F34BC;
extern u8 D_800F34BD;
extern u8 D_800F34BE;
extern s16 D_800F34C0;
extern s16 D_800F34C2;

int func_800C9A70(void) {
    register int half asm("$4");
    register int value asm("$3");
    register void *slotData asm("$3");
    void **slot;

    slot = FieldEng_GetSlot();
    slotData = &D_800E0B38;
    *slot = slotData;

    value = 0xBD;
    D_800E22FC = value;
    value = 9;
    half = 0x80;
    asm volatile("" : "=r"(half) : "0"(half));
    D_800E22FD = value;
    value = 0x80;
    D_800E22F8 = value;
    D_800E22F9 = value;
    D_800E22FA = value;
    value = 0xAC;
    D_800F34BC = value;
    value = 6;
    D_800F34BD = value;
    value = -0x32;
    D_800F34C0 = value;
    value = 0x50;
    D_800E2300 = 0;
    D_800E2302 = half;
    D_800E22FE = 0;
    D_800F34C2 = half;
    D_800F34B8 = value;
    D_800F34B9 = value;
    D_800F34BA = value;
    D_800F34BE = 0;

    return 0;
}
