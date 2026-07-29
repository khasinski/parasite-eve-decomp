typedef unsigned char u8;
typedef short s16;

extern void **FieldEng_GetSlot(void);
extern char D_80196CE0[];
extern u8 D_80197434;
extern u8 D_80197435;
extern u8 D_80197436;
extern s16 D_80197438;
extern s16 D_8019743A;
extern u8 D_80197450;
extern u8 D_80197451;
extern u8 D_80197452;
extern u8 D_80197454;
extern u8 D_80197455;
extern u8 D_80197456;
extern s16 D_80197458;
extern s16 D_8019745A;

int func_8018F254(void) {
    *FieldEng_GetSlot() = D_80196CE0;
    D_80197434 = 0x20;
    D_80197435 = 3;
    D_80197454 = 0x2B;
    D_80197455 = 2;
    D_8019745A = 0x80;
    D_80197438 = 0;
    D_8019743A = 0;
    D_80197436 = 0;
    D_80197458 = 0;
    D_80197450 = 0x80;
    D_80197451 = 0x80;
    D_80197452 = 0x80;
    D_80197456 = 0;
    return 0;
}
