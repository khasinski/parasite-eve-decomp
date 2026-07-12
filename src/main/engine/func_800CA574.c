typedef unsigned char u8;
typedef signed short s16;

void **FieldEng_GetSlot(void);

extern int D_800E0C88;

extern u8 D_800E2308;
extern u8 D_800E2309;
extern u8 D_800E230A;
extern u8 D_800E230C;
extern u8 D_800E230D;
extern u8 D_800E230E;
extern s16 D_800E2310;
extern s16 D_800E2312;

extern u8 D_800E2338;
extern u8 D_800E2339;
extern u8 D_800E233A;
extern u8 D_800E233C;
extern u8 D_800E233D;
extern u8 D_800E233E;
extern s16 D_800E2340;
extern s16 D_800E2342;

extern u8 D_800F34C8;
extern u8 D_800F34C9;
extern u8 D_800F34CA;
extern u8 D_800F34CC;
extern u8 D_800F34CD;
extern u8 D_800F34CE;
extern s16 D_800F34D0;
extern s16 D_800F34D2;

extern u8 D_800F34E8;
extern u8 D_800F34E9;
extern u8 D_800F34EA;
extern u8 D_800F34EC;
extern u8 D_800F34ED;
extern u8 D_800F34EE;
extern s16 D_800F34F0;
extern s16 D_800F34F2;

int func_800CA574(void) {
    register int half_a2 asm("$6");
    register int half_a1 asm("$5");
    register int shade_a0 asm("$4");
    register int value asm("$3");
    register void *slotData asm("$3");
    void **slot;

    slot = FieldEng_GetSlot();
    slotData = &D_800E0C88;
    *slot = slotData;

    value = 0xBD;
    D_800E230C = value;
    value = 9;
    asm volatile(
        "li $6,0x80\n\t"
        "li $5,0x80"
        : "=r"(half_a2), "=r"(half_a1));
    D_800E230D = value;
    value = 0xAE;
    D_800F34CC = value;
    value = 7;
    D_800F34CD = value;
    value = -0x32;
    asm volatile("li $4,0x50" : "=r"(shade_a0));
    D_800F34D0 = value;
    value = 0x68;
    D_800E233C = value;
    value = -0x3C;
    D_800E2340 = value;
    value = 0x42;
    D_800F34EC = value;
    value = 0x20;
    D_800F34ED = value;
    value = 0x32;
    D_800E2310 = 0;
    D_800E2312 = half_a2;
    D_800E2308 = half_a1;
    D_800E2309 = half_a1;
    D_800E230A = half_a1;
    D_800E230E = 0;
    D_800F34D2 = half_a2;
    D_800F34C8 = shade_a0;
    D_800F34C9 = shade_a0;
    D_800F34CA = shade_a0;
    D_800F34CE = 0;
    D_800E233D = 0;
    D_800E2342 = half_a2;
    D_800E2338 = shade_a0;
    D_800E2339 = shade_a0;
    D_800E233A = shade_a0;
    D_800E233E = 0;
    D_800F34F0 = value;
    D_800F34F2 = half_a2;
    D_800F34E8 = half_a1;
    D_800F34E9 = half_a1;
    D_800F34EA = half_a1;
    D_800F34EE = 0;

    return 0;
}
