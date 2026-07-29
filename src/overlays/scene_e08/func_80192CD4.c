typedef short s16;
typedef unsigned short u16;

void func_80192CD4(void *arg0, unsigned char *signal, unsigned char *state) {
    if (*(s16 *)(signal + 2) < 0x3D) {
        *(s16 *)(state + 4) = 1;
    } else {
        *(s16 *)(state + 4) = 2;
    }

    if (*(s16 *)(state + 4) == 1) {
        *(s16 *)(state + 2) += 8;
        if (*(s16 *)(state + 2) >= 0x80) {
            *(s16 *)(state + 2) = 0x7F;
        }
    }

    if (*(s16 *)(state + 4) == 2) {
        *(s16 *)(state + 2) -= 8;
        if (*(s16 *)(state + 2) < 0) {
            *(s16 *)(state + 2) = 0;
        }
    }

    if (*(s16 *)(signal + 2) >= 0x79) {
        signal[1] = 2;
    }
}
