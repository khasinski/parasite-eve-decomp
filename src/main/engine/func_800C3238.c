typedef unsigned char u8;
typedef unsigned short u16;

int GetTPage(int tp, int abr, int x, int y);

extern u8 D_800F33AC;
extern u8 D_800E224C;
extern u8 D_800F33B8;
extern u8 D_800F337A;
extern u16 D_800F3424;
extern u16 D_800F3426;
extern u16 D_800E27AC;

void func_800C3238(u8 mode) {
    D_800F33B8 = mode;

    switch (mode) {
    case 0:
        D_800F337A = 0;
        D_800E224C = 0;
        break;
    case 1:
        D_800F337A = 1;
        D_800E224C = 0;
        break;
    case 2:
        D_800F337A = 1;
        D_800E224C = 1;
        break;
    case 3:
        D_800F337A = 1;
        D_800E224C = 2;
        break;
    case 4:
        D_800F337A = 1;
        D_800E224C = 3;
        break;
    }

    D_800E27AC = GetTPage(D_800F33AC, D_800E224C, D_800F3424, D_800F3426);
}
