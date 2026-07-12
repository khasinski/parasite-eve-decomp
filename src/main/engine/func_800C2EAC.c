typedef unsigned char u8;
typedef unsigned short u16;
typedef signed short s16;

int GetTPage(int tp, int abr, int x, int y);

extern u8 D_800F33AC;
extern u8 D_800E224C;
extern s16 D_800F341C;
extern s16 D_800F341E;
extern u8 D_800F3422;
extern u16 D_800F3424;
extern u16 D_800F3426;
extern u16 D_800E27AC;

void func_800C2EAC(u8 mode) {
    if (mode == 0) {
        D_800F3424 = 0x340;
        D_800F3426 = 0x100;
        D_800F341C = 0;
        D_800F341E = 0x1D7;
        D_800F3422 = 0;
    }
    if (mode == 1) {
        D_800F3424 = 0x340;
        D_800F3426 = 0x160;
        D_800F341E = 0x1DB;
        D_800F341C = 0;
        D_800F3422 = 0x60;
    }
    if (mode == 2) {
        D_800F3424 = 0x340;
        D_800F3426 = 0x100;
        D_800F341C = 0;
        D_800F341E = 0x1D6;
        D_800F3422 = 0;
    }
    if (mode == 3) {
        D_800F3424 = 0x380;
        D_800F3426 = 0x100;
        D_800F341C = 0;
        D_800F341E = 0x1C8;
        D_800F3422 = 0;
    }

    D_800E27AC = GetTPage(D_800F33AC, D_800E224C, D_800F3424, D_800F3426);
}
