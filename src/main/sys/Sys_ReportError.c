extern char D_80010EB0[];
extern char D_80010EC8[];
extern char *D_80091A2C[];

int printf(char *fmt, ...);

void func_80038954(char *arg0, char *arg1, int arg2, unsigned char arg3) {
    int index = arg3 & 0xFF;

    printf(D_80010EB0, D_80091A2C[index], arg0);
    printf(D_80010EC8, arg1, arg2);

    if (index == 1) {
loop:
        goto loop;
    }
}
