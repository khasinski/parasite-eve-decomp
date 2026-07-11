typedef signed short s16;

extern void *D_8009D254;

int *func_800C2B28(int arg0);
int func_800C2B68(void);
void func_80020D50(void);
void func_80020DD0(void *arg0);
int FieldEng_GetStatus(void *arg0);

void func_80191750(char *arg0, unsigned char *arg1) {
    int *status;
    int *flags;
    int *objectFlags;

    status = func_800C2B28(1);
    if (*status == 1) {
        if (*(s16 *)(arg1 + 2) < 0xF0) {
            func_80020D50();
        }

        if (*(s16 *)(arg1 + 2) == 0xF0) {
            if (FieldEng_GetStatus(arg0) == 3) {
                flags = *(int **)D_8009D254;
                flags[0x13] |= 0x4000;

                objectFlags = *(int **)*(int **)(arg0 + 8);
                *objectFlags |= 0x80000000;
                func_80020DD0(objectFlags);
            }
        }
    }

    if (func_800C2B68() == 1) {
        arg1[1] = 2;
    }
}
