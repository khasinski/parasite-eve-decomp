#include "room_m273_effects.h"

typedef struct { unsigned char unknown[0x238]; unsigned char *object; } Scene;
typedef struct { void *position; } Effect;
extern RoomM273EffectStateContext *D_800F32D0;
extern Scene *g_PlayerEntity;
extern void *D_8019AE94;
extern unsigned char D_8019AE9A;
extern int func_801947CC(), func_80199F84();
extern int func_800CE560(void *, int, int, int (*)());
extern int func_800CE5AC(void *, int, int, int, int (*)());
extern Effect *func_800CE610(void *);
extern int func_800CE688(void *), func_800CE78C(void *);

int func_801949EC(int mode) {
    switch (mode) {
    case 0: {
        int size = func_800CE560(D_800F33E0->pool, 8, 2, func_801947CC);
        asm("" : : "r"(size));
        return size + func_800CE5AC(&D_8019AE94, size, 16, 9, func_80199F84);
    }
    case 1: {
        RoomM273EffectModeState *state;
        if (D_8019AE9A) return 2;
        state = D_800F32D0->state.mode;
        if (state->kind == 9) {
            unsigned short value = state->value26;
            if (state->value22 >= 4 && (short)value < 4) {
                Effect *effect = func_800CE610(D_800F33E0->pool);
                if (effect) effect->position = g_PlayerEntity->object + 20;
            }
        }
        func_800CE688(D_8019AE94);
        break;
    }
    case 2:
        func_800CE78C(D_8019AE94);
        break;
    }
    return 0;
}
