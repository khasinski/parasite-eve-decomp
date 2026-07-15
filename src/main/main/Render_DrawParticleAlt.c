typedef unsigned char u8;

extern u8 D_8009B558[];
extern int D_8009B554;
extern void (*D_800A36A4)(int, void *);
extern void (*D_800A36A8)(int, void *);
extern void (*D_800A36AC)(int, void *);

void Render_DrawParticleAlt(int kind, void *arg1) {
    u8 *state;
    int mode;
    int submode;

    state = D_8009B558 + 0x20;

    if (*(int *)state == 0xC) {
        *(int *)state = 0xD;
    }

    if ((kind & 0xFF) == 2) {
        mode = *(int *)state;
        if (mode == 0xD) {
            if (state[-0xC] & 0x10) {
                return;
            }
            *(int *)state = 0xE;
            *(int *)(state - 4) = 2;
            *(int *)(state + 4) = 0x15;
            (*(int *)(state + 0x18))++;
            return;
        }

        if (mode == 0xE) {
            submode = *(int *)(state + 4);
            if (submode == 0x15) {
                if (state[-0xC] & 0x10) {
                    return;
                }
                *(int *)(state - 4) = 2;
                *(int *)state = mode;
                *(int *)(state + 4) = 0x16;
                *(int *)(state + 0x14) = 0;
                return;
            }

            if (submode == 0x16) {
                if (state[-0xC] & 2) {
                    *(int *)(state - 4) = 2;
                    *(int *)state = mode;
                    *(int *)(state + 4) = 0x17;
                    return;
                }
                if (*(int *)(state + 0x14) < 0x12D) {
                    return;
                }
                *(int *)(state - 4) = 3;
                if (D_800A36AC != 0) {
                    D_800A36AC(5, arg1);
                }
                return;
            }

            if (submode == 0x17) {
                *(int *)(state - 4) = 2;
                *(int *)state = mode;
                *(int *)(state + 4) = 0x18;
                return;
            }

            if (submode == 0x18) {
                if (state[-0xC] == 2) {
                    *(int *)(state - 4) = 1;
                    *(int *)state = 0xB;
                    *(int *)(state + 4) = 0;
                    if (D_800A36AC != 0) {
                        D_800A36AC(2, arg1);
                    }
                }
                return;
            }

            return;
        }

        if ((unsigned int)(mode - 0x10) < 2) {
            if (*(int *)(state + 0x28) != 0) {
                return;
            }
            if (state[-0xC] & 2) {
                return;
            }
            *(int *)(state - 4) = 1;
            *(int *)state = 0xB;
            if (D_800A36A8 != 0 && D_8009B554 != 0) {
                D_800A36A8(5, arg1);
            }
            if (D_800A36A4 != 0 && D_8009B554 != 0) {
                D_800A36A4(5, arg1);
            }
        }
    } else {
        if (state[-0xC] & 0x10) {
            *(int *)(state - 4) = 2;
            *(int *)state = 0xD;
        } else if ((unsigned int)(*(int *)state - 0x10) < 2) {
            *(int *)(state - 4) = 1;
            *(int *)state = 0xB;
        }
    }
}
