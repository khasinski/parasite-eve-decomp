typedef unsigned char u8;
typedef unsigned short u16;
typedef signed short s16;

void *func_800C2B90(void *obj, u8 id, u16 *sizes, int (**handlers)(void *, void *, void *));
int func_800C2E08(void);

extern u8 *D_800F34F4;
extern char *D_800E2248;
extern char *D_800F32A8;
extern char *D_800F3330;
extern int *D_800F33B0;

int func_800C2758(char *obj, int (**handlers)(void *, void *, void *), u16 *sizes) {
    int result = 0;
    u8 finished = 0;
    u8 stopped = 0;
    s16 timer;

    D_800E2248 = obj + 0xC;
    D_800F34F4 = (u8 *)(obj + 0x80);
    D_800F32A8 = obj;
    D_800F3330 = obj + 0x200;
    D_800F33B0 = *(int **)(obj + 0x78);

    timer = *(s16 *)(obj + 0xC);
    if (timer != 0) {
        *(s16 *)(obj + 0xC) = timer - 1;
        return result;
    }

    while (!finished) {
        int command = D_800F33B0[*(s16 *)(D_800E2248 + 2)];

        if (command == -3) {
            result |= func_800C2E08();
        }

        if (command == -2) {
            command = 0;
        }

        if (command == -1) {
            stopped = 1;
            D_800E2248[7] = 1;
        } else {
            unsigned int high = command >> 16;
            unsigned int op = high;
            u16 lowOp = op;
            int value = command;

            if (lowOp == 1) {
                func_800C2B90(obj, value & 0xFF, sizes, handlers);
            }

            if (lowOp == 2) {
                stopped = 1;
                *(s16 *)D_800E2248 = value;
            }

            if (high - 0x10 < 0x10) {
                *(int *)(D_800E2248 + (((op - 0x10) & 0xFF) * 4) + 8) = (s16)command;
            }

            if (high - 0x20 < 0x10) {
                char *slot = D_800E2248 + (((op - 0x20) & 0xFF) * 4);

                *(int *)(slot + 8) += (s16)command;
            }

            if (high - 0x30 < 0x10) {
                char *slot = D_800E2248 + (((op - 0x30) & 0xFF) * 4);

                *(int *)(slot + 8) -= (s16)command;
            }

            if (high - 0x40 < 0x10) {
                *(int *)(D_800E2248 + (((op - 0x40) & 0xFF) * 4) + 8) =
                    *(int *)(D_800E2248 + ((value & 0xFFFF) * 4) + 0x48);
            }

            if (high - 0x50 < 0x10) {
                *(int *)(D_800E2248 + ((value & 0xFFFF) * 4) + 0x48) =
                    *(int *)(D_800E2248 + (((op - 0x50) & 0xFF) * 4) + 8);
            }

            if (high - 0x1000 < 0x1000) {
                if (*(int *)(D_800E2248 + ((high & 0xF00) >> 6) + 8) == (s16)command) {
                    *(u16 *)(D_800E2248 + 2) += (signed char)op;
                }
            }

            if (op - 0x2000 < 0x1000) {
                if (*(int *)(D_800E2248 + ((op & 0xF00) >> 6) + 8) != (s16)value) {
                    *(u16 *)(D_800E2248 + 2) += (signed char)op;
                }
            }

            if (op - 0x3000 < 0x1000) {
                *(u16 *)(D_800E2248 + 2) += (signed char)op;
            }
        }

        if ((signed char)D_800E2248[7] == 0) {
            *(u16 *)(D_800E2248 + 2) += 1;
        } else if ((signed char)D_800E2248[6] == 0) {
            result = -1;
        }

        if (stopped) {
            finished = 1;
        }
    }

    return result;
}
