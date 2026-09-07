/* MASPSX_FLAGS: --expand-div */
typedef struct { short x, y, z, pad; } Vector;
typedef struct { short rotation[3][3]; int position[3]; } Matrix;
typedef struct { int color; short position[3], size; } Particle;
typedef struct { char reserved[0x238]; Matrix *transforms; } Instance;
typedef struct { int reserved[2]; void *pool; } Emitter;
extern Emitter *D_800F33E0;
extern Instance *D_8019A7F8;
extern int D_800E27EC, D_800966EC[];
extern volatile short D_800F3368, D_800F336A, D_800F3376, D_800F3378;
extern volatile short D_800F336C, D_800F336E, D_800F3372, D_800F3374;
extern volatile unsigned short D_800E11E8, D_800F3370;
extern unsigned short D_800E2850[];
extern int func_8019404C(int, Particle *);
extern int func_800CE560(void *, int, int, int (*)(int, Particle *));
extern Particle *func_800CE610(void *);
extern int Inv_ScrambleGrid(void);
extern void RotMatrixYXZ(Vector *, Matrix *);
extern void ApplyMatrixSV(Matrix *, Vector *, Vector *);

int func_8019421C(int event)
{
    Vector offset;
    Matrix matrix;
    Vector vector;
    if (event == 1) goto update;
    if (event < 2) { if (event == 0) goto setup; goto done; }
    if (event == 2) goto configure;
    goto done;
setup:
    return func_800CE560(D_800F33E0->pool, 12, 64, func_8019404C);
update:
    {
        int i = 0;
        Vector *out;
        if (D_800E27EC >= 41) return 2;
        out = &offset;
        for (; i < 2; i++) {
            register int random asm("$16");
            int r, g, b, divisor;
            int j, *position;
            Particle *particle = func_800CE610(D_800F33E0->pool);
            if (!particle) return 0;
            vector.x = (unsigned int)Inv_ScrambleGrid() << 4;
            vector.y = (unsigned int)Inv_ScrambleGrid() << 4;
            vector.z = 0;
            RotMatrixYXZ(&vector, &matrix);
            asm volatile("" : : "i"(&&offset_start));
offset_start:
            vector.x = 0; vector.y = 0;
            random = Inv_ScrambleGrid();
            random = ((unsigned int)random << 8) | (unsigned int)Inv_ScrambleGrid();
            divisor = 192;
            vector.z = random % divisor + 128;
            ApplyMatrixSV(&matrix, &vector, out);
            position = D_8019A7F8->transforms[i ? 11 : 15].position;
            for (j = 0; j < 3; j++)
                particle->position[j] = ((short *)out)[j] + (unsigned int)position[j];
            particle->size = (unsigned int)D_800966EC[
                ((int)((unsigned int)D_800E27EC << 11) / 40) & 4095] + 4096;
            r = Inv_ScrambleGrid(); g = Inv_ScrambleGrid(); b = Inv_ScrambleGrid();
            particle->color = (r >> 1) | ((unsigned int)(g >> 1) << 8) |
                ((unsigned int)(b >> 1) << 16);
        }
    }
    goto done;
configure:
    {
        int index = D_800E11E8;
        int palette;
        D_800F3368 = 16; D_800F336A = 1;
        D_800F3376 = 16; D_800F3378 = 16;
        D_800F3376 = 16; D_800F3378 = 16;
        palette = D_800E2850[index];
        D_800F336C = 2; D_800F336E = 0;
        D_800F3372 = 0; D_800F3374 = 0; D_800F3370 = palette;
    }
done:
    return 0;
}
