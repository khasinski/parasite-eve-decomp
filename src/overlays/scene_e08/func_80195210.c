#include "common.h"
#include "pe1/random.h"

typedef struct {
    u8 pad_00[8];
    s16 field_08;
    s16 field_0A;
    s16 offsets[12];
    s16 variation[3];
} Ovl178RandomizedEffect;

typedef struct {
    u8 pad_00;
    u8 mode;
    s16 timer;
} Ovl178EffectController;

void func_80195210(void *unused, Ovl178EffectController *state,
                   Ovl178RandomizedEffect *effect) {
    unsigned i;

    if (effect->field_08 < 3000) {
        effect->field_08 += 50;
    }
    if (effect->field_0A < 128) {
        effect->field_0A += 8;
    }

    for (i = 0; i < 12; i++) {
        effect->offsets[i] = Engine_Random() % 400 - 200;
    }
    for (i = 0; i < 3; i++) {
        effect->variation[i] = Engine_Random() % 300 - 150;
    }
    if (state->timer >= 36) {
        state->mode = 2;
    }
}
