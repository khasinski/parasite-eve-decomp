/* MASPSX_FLAGS: --stack-return-delay */

extern unsigned char D_8009574E[];
extern void (*D_80095748[])(char *msg, int arg0);
extern char D_80011854[];

int DrawSyncCallback(int arg0) {
    register unsigned char *base asm("$16");
    register int saved asm("$17");
    int old;

    base = D_8009574E;
    asm volatile("" : "=r"(base) : "0"(base));
    saved = arg0;
    if (base[0] >= 2) {
        D_80095748[0](D_80011854, saved);
    }

    asm volatile("" : "=r"(base) : "0"(base));
    old = *(int *)(base + 0xA);
    *(int *)(base + 0xA) = saved;
    return old;
}
