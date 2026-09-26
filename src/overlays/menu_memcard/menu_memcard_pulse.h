#ifndef MENU_MEMCARD_PULSE_H
#define MENU_MEMCARD_PULSE_H

#include "pe1/render_object.h"
typedef struct MemcardPulseState {
    GteShortVector position;
    GteRotation rotation;
    s16 phase, timer;
} MemcardPulseState;
extern s32 func_80077CF4(s32);
extern s32 func_80077DC4(s32);
extern u16 func_80077AA4(s32, s32);
extern u8 D_801F1D5C[];
extern RenderColor D_801F1F30;
extern RenderColor D_801F1F34;

s32 Memcard_PulseImage(s32 mode, MemcardPulseState *state);
s32 Memcard_CosineImage(s32 mode, MemcardPulseState *state);
s32 Memcard_SineRotatingImage(s32 mode, MemcardPulseState *state);

#endif
