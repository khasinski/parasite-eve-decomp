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
extern void *jtbl_800C2128[];

void func_800C3238(u8 mode) {
    D_800F33B8 = mode;

    asm("" : : "X"(&&case0), "X"(&&case1), "X"(&&case2), "X"(&&case3), "X"(&&case4));

    if (mode < 5) {
        goto *jtbl_800C2128[mode];
    }
    goto done;

case0:
        D_800F337A = 0;
        D_800E224C = 0;
        goto done;
case1:
        D_800F337A = 1;
        D_800E224C = 0;
        goto done;
case2:
        D_800F337A = 1;
        D_800E224C = 1;
        goto done;
case3:
        D_800F337A = 1;
        D_800E224C = 2;
        goto done;
case4:
        D_800F337A = 1;
        D_800E224C = 3;

done:
    D_800E27AC = GetTPage(D_800F33AC, D_800E224C, D_800F3424, D_800F3426);
}
