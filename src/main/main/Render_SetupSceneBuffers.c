/* MASPSX_FLAGS: --stack-return-delay */

extern volatile unsigned int *D_800956BC;
extern void (*D_800956C0[])(void);
extern unsigned int *D_800956E0;
extern char D_8001177C[];
extern char D_80011798[];

int printf(char *fmt, ...);

void Render_SetupSceneBuffers(void) {
    register unsigned int bits asm("$17");
    register int i asm("$16");
    register void (**callbacks)(void) asm("$18");
    register unsigned int mask_base asm("$19");
    register int one asm("$20");
    register void (**callback_base)(void) asm("$21");
    volatile unsigned int *reg;
    unsigned int value;

    reg = D_800956BC;
    bits = (*reg >> 24) & 0x7F;
    if (bits != 0) {
        one = 1;
        mask_base = 0xFFFFFF;
        callback_base = D_800956C0;
        do {
            if (bits != 0) {
                i = 0;
                callbacks = callback_base;
                do {
                    if (i >= 7) {
                        break;
                    }
                    if (bits & 1) {
                        reg = D_800956BC;
                        *reg &= (one << (i + 0x18)) | mask_base;
                        if (*callbacks != 0) {
                            (*callbacks)();
                        }
                    }
                    callbacks++;
                    bits >>= 1;
                    i++;
                } while (bits != 0);
            }
            reg = D_800956BC;
            bits = (*reg >> 24) & 0x7F;
        } while (bits != 0);
    }

    reg = D_800956BC;
    value = *reg & 0xFF000000;
    if (value == 0x80000000 || (*reg & 0x8000) != 0) {
        printf(D_8001177C, *reg);
        for (i = 0; i < 7; i++) {
            printf(D_80011798, i, *(unsigned int *)((char *)D_800956E0 + (i << 4)));
        }
    }
}
