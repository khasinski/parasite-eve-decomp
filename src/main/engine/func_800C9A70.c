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
    int half = 0x80;

    *FieldEng_GetSlot() = &D_800E0B38;

    D_800E22FC = 0xBD;
    D_800E22FD = 9;
    D_800E22F8 = 0x80;
    D_800E22F9 = 0x80;
    D_800E22FA = 0x80;
    D_800F34BC = 0xAC;
    D_800F34BD = 6;
    D_800F34C0 = -0x32;
    D_800E2300 = 0;
    D_800E2302 = half;
    D_800E22FE = 0;
    D_800F34C2 = half;
    D_800F34B8 = 0x50;
    D_800F34B9 = 0x50;
    D_800F34BA = 0x50;
    D_800F34BE = 0;

    return 0;
}
