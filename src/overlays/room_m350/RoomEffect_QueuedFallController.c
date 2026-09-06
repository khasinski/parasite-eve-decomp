typedef struct { int reserved[2]; void *pool; } Emitter;
extern Emitter *D_800F33E0;
extern unsigned char D_8019A86E, D_8019A857;
extern short D_8019A864, D_8019A866, D_8019A868;
extern unsigned short D_800E11FA, D_800E2850[];
extern short D_800F3368, D_800F336A, D_800F336E;
extern unsigned short D_800F336C, D_800F3370;
extern short D_800F3372, D_800F3374;
/* Retail writes both fields twice during configuration. */
extern volatile short D_800F3376, D_800F3378;
extern int func_80197594(int, short *);
extern int func_800CE560(void *, int, int, int (*)(int, short *));
extern short *func_800CE610(void *);

int func_801976C8(int event)
{
    if (event == 1) goto update;
    if (event < 2) {
        if (event == 0) goto setup;
        goto done;
    }
    if (event == 2) goto configure;
    goto done;
setup:
    return func_800CE560(D_800F33E0->pool, 8, 4, func_80197594);
update:
    if (D_8019A86E) return 2;
    if (D_8019A857) {
        short *effect = func_800CE610(D_800F33E0->pool);
        if (!effect) return 0;
        effect[0] = D_8019A864;
        effect[1] = D_8019A866;
        asm("" : "=m"(D_8019A868) : "m"(D_8019A868), "m"(effect[1]));
        effect[2] = D_8019A868;
        effect[3] = 0;
        /* Clear the request only after its position has been copied. */
        asm volatile("" : "=m"(D_8019A857) : "m"(D_8019A857), "m"(effect[2]));
        D_8019A857 = 0;
    }
    goto done;
configure:
    {
        int unit = 32;
        int index = D_800E11FA;
        int palette;
        D_800F3368 = unit;
        D_800F336A = 2;
        D_800F3376 = unit;
        D_800F3378 = unit;
        D_800F3376 = unit;
        D_800F3378 = unit;
        asm volatile("" : "=r"(index) : "0"(index));
        palette = D_800E2850[index];
        D_800F336C = 3;
        D_800F336E = 1;
        D_800F3372 = 0;
        D_800F3374 = 0;
        D_800F3370 = palette;
    }
done:
    return 0;
}
