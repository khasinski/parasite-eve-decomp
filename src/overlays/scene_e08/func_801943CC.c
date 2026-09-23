#include "scene_e08.h"

void func_801943CC(void *arg0, void *arg1, Ovl178EffectState *out)
{
    Ovl178Position *position = func_800C2B50();

    out->field_00 = position->x << 16;
    out->field_04 = position->y << 16;
    out->field_08 = position->z << 16;
    out->field_20 = 0;
    out->field_22 = 0;
    out->field_24 = 0;
    out->field_2C = *func_800C2B10(2);
    out->field_28 = *func_800C2B10(3);
    out->field_2A = *func_800C2B28(1);
    if (out->field_28 == 0)
        out->field_18 = 0x124F80;
    else
        out->field_18 = 0xFFEDB080;
    out->field_10 = 0x7A120;
    out->field_14 = 0xFFF551A0;
}
