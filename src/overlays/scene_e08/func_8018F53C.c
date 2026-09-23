#include "scene_e08.h"

void func_8018F53C(void *unused0, void *unused1, Ovl178FadeState *state)
{
    unsigned i;
    int offset;

    state->field_264 = 0x80;
    i = 0;
    offset = 0;
    for (; i < 4; i++) {
        state->field_254[i] = offset;
        offset += 0x12C;
        state->field_25C[i] = i << 10;
    }

    for (i = 0; i < 2; i++) {
        RoomFxEmitterParams *item = &state->emitters[i];

        state->field_230[i] = (i + 1) * 700;
        item->mode = 0x10;
        item->intensity = 0x80;
        item->color1[0] = 0xC8;
        item->color1[1] = 0xFF;
        item->extent0 = 0x514;
        item->extent1 = 0x320;
        item->offset = 0;
        item->color1[2] = 0;
        item->color0[0] = 0;
        item->color0[1] = 0;
        item->color0[2] = 0;
        item->source = state->sourceData[i];
        func_800C4E50(item);
    }
}
