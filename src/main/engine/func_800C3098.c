typedef unsigned char u8;
typedef unsigned short u16;

int GetTPage(int tp, int abr, int x, int y);
int printf(const char *fmt, ...);

extern char D_800C2110[];
extern u8 D_800F33AC;
extern u8 D_800E224C;
extern u16 D_800F3424;
extern u16 D_800F3426;
extern u16 D_800E27AC;

void func_800C3098(int mode) {
    mode = (short)mode;

    if (mode == 0x10) {
        goto mode16;
    }
    if (mode == 0x100) {
        goto mode256;
    }
    goto badMode;

mode16:
    D_800F33AC = 0;
    goto done;

mode256:
    D_800F33AC = 1;
    goto done;

badMode:
    printf(D_800C2110);

done:
    D_800E27AC = GetTPage(D_800F33AC, D_800E224C, D_800F3424, D_800F3426);
}
