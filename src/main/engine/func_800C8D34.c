typedef unsigned char u8;
typedef signed short s16;

void **FieldEng_GetSlot(void);

extern int D_800E0A50;

extern u8 D_800E22E8;
extern u8 D_800E22E9;
extern u8 D_800E22EA;
extern u8 D_800E22EC;
extern u8 D_800E22ED;
extern u8 D_800E22EE;
extern s16 D_800E22F0;
extern s16 D_800E22F2;

extern u8 D_800F34A8;
extern u8 D_800F34A9;
extern u8 D_800F34AA;
extern u8 D_800F34AC;
extern u8 D_800F34AD;
extern u8 D_800F34AE;
extern s16 D_800F34B0;
extern s16 D_800F34B2;

extern u8 D_800E2328;
extern u8 D_800E2329;
extern u8 D_800E232A;
extern u8 D_800E232C;
extern u8 D_800E232D;
extern u8 D_800E232E;
extern s16 D_800E2330;
extern s16 D_800E2332;

int func_800C8D34(void) {
    register int half asm("$5");
    register int shade asm("$4");
    register int value asm("$3");
    register void *slotData asm("$3");
    void **slot;

    slot = FieldEng_GetSlot();
    slotData = &D_800E0A50;
    *slot = slotData;

    value = 0xBD;
    D_800E22EC = value;
    value = 9;
    half = 0x80;
    asm volatile("" : "=r"(half) : "0"(half));
    D_800E22ED = value;
    value = 0x80;
    D_800E22E8 = value;
    D_800E22E9 = value;
    D_800E22EA = value;
    value = 0xAE;
    D_800F34AC = value;
    value = 7;
    D_800F34AD = value;
    value = -0x32;
    shade = 0x50;
    asm volatile("" : "=r"(shade) : "0"(shade));
    D_800F34B0 = value;
    D_800E22F0 = 0;
    D_800E22F2 = half;
    D_800E22EE = 0;
    D_800F34B2 = half;
    D_800F34A8 = shade;
    D_800F34A9 = shade;
    D_800F34AA = shade;
    D_800F34AE = 0;
    value = 0x68;
    D_800E232C = value;
    D_800E232D = 0;
    D_800E2330 = 0;
    D_800E2332 = half;
    D_800E2328 = shade;
    D_800E2329 = shade;
    D_800E232A = shade;
    D_800E232E = 0;

    return 0;
}
