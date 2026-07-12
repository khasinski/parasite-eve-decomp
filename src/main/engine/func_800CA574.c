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
    int half = 0x80;
    int shade = 0x50;

    *FieldEng_GetSlot() = &D_800E0C88;

    D_800E230C = 0xBD;
    D_800E230D = 9;
    D_800F34CC = 0xAE;
    D_800F34CD = 7;
    D_800F34D0 = -0x32;
    D_800E233C = 0x68;
    D_800E2340 = -0x3C;
    D_800F34EC = 0x42;
    D_800F34ED = 0x20;
    D_800E2310 = 0;
    D_800E2312 = half;
    D_800E2308 = half;
    D_800E2309 = half;
    D_800E230A = half;
    D_800E230E = 0;
    D_800F34D2 = half;
    D_800F34C8 = shade;
    D_800F34C9 = shade;
    D_800F34CA = shade;
    D_800F34CE = 0;
    D_800E233D = 0;
    D_800E2342 = half;
    D_800E2338 = shade;
    D_800E2339 = shade;
    D_800E233A = shade;
    D_800E233E = 0;
    D_800F34F0 = 0x32;
    D_800F34F2 = half;
    D_800F34E8 = half;
    D_800F34E9 = half;
    D_800F34EA = half;
    D_800F34EE = 0;

    return 0;
}
