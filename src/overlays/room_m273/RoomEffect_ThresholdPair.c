typedef struct { unsigned char unknown[8]; void *pool; } Context;
typedef struct {
    unsigned char unknown[14], kind, unknown15[7];
    short value22; unsigned char unknown24[2]; unsigned short value26;
} State;
typedef struct { unsigned char unknown[8]; State *state; } StateContext;
typedef struct { unsigned char unknown[2]; unsigned short angle; unsigned char unknown4[4]; } Seed;
extern Context *D_800F33E0;
extern StateContext *D_800F32D0;
extern unsigned char D_8019AE9A;
extern Seed D_8019AC20[2];
extern unsigned short D_800E11FA, D_800E2850[];
extern unsigned short D_800F3368, D_800F336A, D_800F336C, D_800F336E;
extern unsigned short D_800F3370, D_800F3372, D_800F3374;
extern volatile unsigned short D_800F3376, D_800F3378;
extern int func_80194128();
extern int func_800CE560(void *, int, int, int (*)());
extern Seed *func_800CE610(void *);
extern int Inv_ScrambleGrid(void);

int func_80194284(int mode) {
    switch (mode) {
    case 0:
        return func_800CE560(D_800F33E0->pool, 8, 4, func_80194128);
    case 1: {
        State *state;
        if (D_8019AE9A) return 2;
        state = D_800F32D0->state;
        if (state->kind == 9) {
            unsigned short value = state->value26;
            if (state->value22 > 0 && (short)value <= 0) {
                short i = 0;
                int angle = Inv_ScrambleGrid() << 4;
                do {
                    Seed *seed = func_800CE610(D_800F33E0->pool);
                    if (!seed) break;
                    *seed = D_8019AC20[i];
                    i++;
                    seed->angle += angle;
                } while (i < 2);
            }
        }
        break;
    }
    case 2: {
        int palette;
        D_800F3368 = 16;
        D_800F3376 = 16; D_800F3378 = 16; D_800F3376 = 16;
        palette = D_800E2850[D_800E11FA];
        D_800F336A = 1; D_800F3378 = 64;
        asm("" : : : "memory");
        D_800F336C = 3; D_800F336E = 1; D_800F3372 = 3; D_800F3374 = 0;
        D_800F3370 = palette;
        break;
    }
    }
    return 0;
}
