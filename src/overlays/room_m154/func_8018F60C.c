typedef signed short s16;
typedef unsigned short u16;
typedef unsigned char u8;

extern int **D_8009D254;

int FieldEng_GetStatus(void *arg0);
int func_800C2B68(void);

void func_8018F60C(void *arg0, u8 *arg1, char *arg2) {
    char *work;
    int **flagsBase;
    int *flags;
    int *objectFlags;
    u16 timer;

    work = arg2;
    if (*(s16 *)(arg2 + 0x2A) == 0) {
        if (*(s16 *)(work + 0x2C) == 1) {
            if (FieldEng_GetStatus(arg0) == 3) {
                if (*(u8 *)(*(char **)(*(char **)*(char **)(arg0 + 8) + 0x18)) == 2) {
                    timer = *(u16 *)(work + 0x28);
                    flagsBase = D_8009D254;
                    *(s16 *)(work + 0x2C) = 0;
                    *(s16 *)(arg2 + 0x2A) = timer;

                    flags = *flagsBase;
                    flags[0x13] |= 0x4000;

                    objectFlags = *(int **)*(int **)(arg0 + 8);
                    *objectFlags |= 0x80000000;
                }
            }
        }
    } else {
        *(s16 *)(work + 0x2A) = *(s16 *)(arg2 + 0x2A) - 1;
        *(s16 *)(work + 0x2C) = 0;
    }

    if (func_800C2B68() == 1) {
        arg1[1] = 2;
    }
}
