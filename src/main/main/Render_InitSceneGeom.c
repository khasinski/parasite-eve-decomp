typedef unsigned short u16;

extern u16 D_800945E4;
extern u16 D_800945E6;
extern u16 D_80094614;
extern volatile int D_8009567C;
extern u16 *D_80095670;
extern u16 *D_80095674;
extern void (*D_800945E8[])(void);
extern char D_80011740[];
extern char D_8001175C[];

void printf(char *fmt, ...);
void ReturnFromException(void);

void Render_InitSceneGeom(void) {
    register u16 *state asm("$17") = &D_800945E4;
    register int mask asm("$16");
    register int bit asm("$19");
    register void (**callback)(void) asm("$18");
    register void (**callback_base)(void) asm("$20");
    int index;
    int pending;
    int counter;

    asm volatile("" : "=r"(state) : "0"(state));
    if (state[0] == 0) {
        printf(D_80011740, *D_80095670);
        ReturnFromException();
    }

    state[1] = 1;
    pending = state[0x18] & *D_80095670 & *D_80095674;
    if (pending != 0) {
        bit = 1;
        callback_base = D_800945E8;
        do {
            mask = pending;
            if (mask != 0) {
                index = 0;
                callback = callback_base;
                do {
                    if (index >= 0xB) {
                        break;
                    }
                    if (mask & 1) {
                        *D_80095670 = ~(bit << index);
                        if (*callback != 0) {
                            (*callback)();
                        }
                    }
                    callback++;
                    mask >>= 1;
                    index++;
                } while ((mask & 0xFFFF) != 0);
            }

            pending = D_80094614 & *D_80095670 & *D_80095674;
        } while (pending != 0);
    }

    if ((*D_80095670 & *D_80095674) != 0) {
        counter = D_8009567C++;
        if (counter >= 0x801) {
            printf(D_8001175C, *D_80095670, *D_80095674);
            D_8009567C = 0;
            *D_80095670 = 0;
        }
    } else {
        D_8009567C = 0;
    }

    D_800945E6 = 0;
    ReturnFromException();
}
