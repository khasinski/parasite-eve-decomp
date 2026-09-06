/* MASPSX_FLAGS: --expand-div */
typedef struct { int reserved[2]; void *pool; } Emitter;
typedef struct { short rotation[3][3]; int translation[3]; } Matrix;
typedef struct { short x, y, z, pad; } Vector;
extern Emitter *D_800F33E0;
extern unsigned char D_8019A86E, D_8019A859;
extern unsigned short D_800E11E8, D_800E2850[];
extern short D_800F3368, D_800F336A, D_800F336E;
extern unsigned short D_800F336C, D_800F3370;
extern short D_800F3372, D_800F3374;
/* Retail writes both fields twice during configuration. */
extern volatile short D_800F3376, D_800F3378;
extern int func_80197A04(int, short *);
extern int func_800CE560(void *, int, int, int (*)(int, short *));
extern short *func_800CE610(void *);
extern int Inv_ScrambleGrid(void);
extern Matrix *RotMatrixYXZ(Vector *, Matrix *);
extern Vector *ApplyMatrixSV(Matrix *, Vector *, Vector *);

int func_80197B98(int event)
{
    if (event == 1) goto update;
    if (event < 2) {
        if (event == 0) goto setup;
        goto done;
    }
    if (event == 2) goto configure;
    goto done;
setup:
    return func_800CE560(D_800F33E0->pool, 8, 16, func_80197A04);
update:
    if (D_8019A86E) return 2;
    if (D_8019A859) {
        register int i asm("$18") = 0;
        do {
            Matrix matrix;
            Vector vector;
            register int random asm("$16");
            int divisor;
            short *effect = func_800CE610(D_800F33E0->pool);
            if (!effect) break;
            i++;
            vector.x = Inv_ScrambleGrid() << 4;
            vector.y = Inv_ScrambleGrid() << 4;
            vector.z = 0;
            RotMatrixYXZ(&vector, &matrix);
            /* Keep the two phases separate in CSE without emitting instructions. */
            asm volatile("" : : "i"(&&offset));
offset:
            vector.x = 0;
            vector.y = 0;
            random = Inv_ScrambleGrid();
            random = (random << 8) | Inv_ScrambleGrid();
            divisor = 64;
            vector.z = random % divisor + 256;
            ApplyMatrixSV(&matrix, &vector, (Vector *)effect);
        } while (i < 2);
        {
            unsigned char *request = &D_8019A859;
            *request = *request - 1;
        }
    }
    goto done;
configure:
    {
        int unit = 16;
        int index = D_800E11E8;
        int palette;
        D_800F3372 = 0;
        D_800F3368 = unit;
        D_800F336A = 1;
        D_800F3376 = unit;
        D_800F3378 = unit;
        D_800F3376 = unit;
        D_800F3378 = unit;
        asm volatile("" : "=r"(index) : "0"(index));
        palette = D_800E2850[index];
        D_800F336C = 2;
        D_800F336E = 0;
        D_800F3374 = 0;
        D_800F3370 = palette;
    }
done:
    return 0;
}
