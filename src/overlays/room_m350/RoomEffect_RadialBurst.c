typedef struct { char reserved[0xF4]; int position[3]; } Transform;
typedef struct { char reserved[0x238]; Transform *transform; } Instance;
typedef struct { int reserved[2]; Instance *instance; } Actor;

extern Actor *D_800F32D0;
extern int D_800E27EC, D_800F3428, D_800966EC[], D_8019A634[];
extern unsigned short D_800F336C, D_800E1204[];
extern short D_800F336A;
extern int GetClut(int, int);
extern void func_800CEE20(void *, int, int, int, int, unsigned int, int, int, void *);

int func_80197A04(int event, short *object)
{
    short position[4];
    if (event == 1) {
        if (D_800E27EC >= 8) {
            return 1;
        }
    } else if (event == 2) {
        int scale = (4096 - *(short *)((char *)D_800966EC +
            (((unsigned int)(D_800E27EC - 1) << 9) & 0x3E00))) * 2;
        int *origin = D_800F32D0->instance->transform->position;
        int i;
        int kind, palette, handle;

        for (i = 0; i < 3; i++) {
            position[i] = object[i] * scale / 4096 + origin[i];
        }
        kind = D_800F336C;
        palette = D_800E1204[kind];
        if (kind == 4 && D_800F3428) {
            palette += 4;
        }
        handle = GetClut(32, palette);
        func_800CEE20(position, 0, 5120, 5120, D_800F336A * 2 + 216,
            (unsigned short)handle, 1,
            (short)D_800966EC[((unsigned int)(D_800E27EC - 1) << 7) & 0xF80] >> 5,
            D_8019A634);
    }
    return 0;
}
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
