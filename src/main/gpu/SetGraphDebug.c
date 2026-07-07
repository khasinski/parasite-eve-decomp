/* MASPSX_FLAGS: -G8 --stack-return-delay --la-call-delay */

typedef void (*DebugCallback)(char *, int, int, int);

extern unsigned char D_8009574E[];
extern DebugCallback D_80095748[];
extern char D_80011814[];

int SetGraphDebug(int debug) {
    register int old asm("$16");
    register unsigned char *slot asm("$3");

    slot = D_8009574E;
    old = *slot;
    *slot = debug;
    debug = (unsigned char)debug;
    if (debug != 0) {
        asm volatile("" : "=r"(slot) : "0"(slot));
        D_80095748[0](D_80011814, slot[0], slot[-2], slot[1]);
    }
    return old;
}
