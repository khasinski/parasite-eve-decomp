typedef struct { int reserved[2]; void *pool; } Emitter;
typedef struct { short x, y, z, pad; } Vector;
extern Emitter *D_800F33E0;
extern unsigned char D_8019A86E;
extern short D_8019A85C;
extern Vector g_RoomEffectTrailPositions[4];
extern unsigned short D_800E11E8, D_800E2850[];
extern short D_800F3368, D_800F336A, D_800F336E, D_800F3372, D_800F3374;
extern unsigned short D_800F336C, D_800F3370;
/* Retail writes both fields twice during configuration. */
extern volatile short D_800F3376, D_800F3378;
extern int func_8019A014(int, short *);
extern int func_800CE560(void *, int, int, int (*)(int, short *));
extern short *func_800CE610(void *);

int func_8019784C(int event)
{
    if (event == 1) goto update;
    if (event < 2) {
        if (event == 0) goto setup;
        goto done;
    }
    if (event == 2) goto configure;
    goto done;
setup:
    return func_800CE560(D_800F33E0->pool, 8, 16, func_8019A014);
update:
    if (D_8019A86E) return 2;
    {
        int i;
        for (i = 0; i < D_8019A85C; i++) {
            short *effect = func_800CE610(D_800F33E0->pool);
            if (!effect) break;
            effect[0] = g_RoomEffectTrailPositions[i].x;
            effect[1] = g_RoomEffectTrailPositions[i].y;
            effect[2] = g_RoomEffectTrailPositions[i].z;
            /* Finish the copy before reloading the possibly aliased count. */
            asm("" : "=m"(D_8019A85C) : "m"(D_8019A85C), "m"(effect[2]));
        }
        D_8019A85C = 0;
    }
    goto done;
configure:
    {
        int unit = 16;
        int index = D_800E11E8;
        int palette;
        D_800F3368 = unit;
        D_800F336A = 1;
        D_800F3376 = unit;
        D_800F3378 = unit;
        D_800F3376 = unit;
        D_800F3378 = unit;
        palette = D_800E2850[index];
        D_800F336C = 2;
        D_800F336E = 0;
        D_800F3372 = 0;
        D_800F3374 = 0;
        D_800F3370 = palette;
    }
done:
    return 0;
}
