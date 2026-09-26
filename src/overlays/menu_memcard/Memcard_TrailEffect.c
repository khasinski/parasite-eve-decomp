#include "menu_memcard_trail.h"

s32 Memcard_TrailEffect(s32 mode, MemcardTrailState *state) {
    GteShortVector position;
    MemcardLinkOffset offset = D_801ED818;
    RenderColor color;
    s32 scale, palette;
    s32 index;
    s32 four;
    switch(mode) {
    case 0:
        state->attachment = 19; state->phase = 0; state->timer = 0;
        return func_800CE560(D_800F33E0->slots, 20, 24, Memcard_CosineImage);
    case 1:
        state->timer++;
        func_800CE8F0(D_8009D254, state->attachment, &offset, &position);
        if (state->timer < 33 && (state->timer & 1)) {
            MemcardPulseState *image = func_800CE610(D_800F33E0->slots);
            if (image) {
                image->position.x = position.x;
                image->position.y = position.y;
                image->position.z = position.z;
                image->phase = 0; image->timer = 0;
            }
        }
        if (state->timer < 32) return 0;
        __asm__ volatile("" : : : "memory");
        return 2;
    case 2:
        {
            RenderMatrixSlot *slot = &D_800BCFA4;
            register s32 *matrix asm("$8");
            register s32 a asm("$12");
            register s32 b asm("$13");
            register s32 c asm("$14");
            __asm__("" : "=r"(slot) : "0"(slot));
            matrix = slot->value;
            a=matrix[0]; b=matrix[1]; gte_ctc2_0(a); gte_ctc2_1(b);
            a=matrix[2]; b=matrix[3]; c=matrix[4];
            gte_ctc2_2(a); gte_ctc2_3(b); gte_ctc2_4(c);
            a=matrix[5]; b=matrix[6]; gte_ctc2_5(a);
            c=matrix[7]; gte_ctc2_6(b); gte_ctc2_7(c);
        }
        func_800CE8F0(D_8009D254, state->attachment, &offset, &position);
        D_800F3368.parameter00 = 32;
        D_800F3368.parameter02 = 2;
        D_800F3368.extent_x = 32; D_800F3368.extent_y = 32;
        D_800F336C = 1;
        D_800F3368.tpage = D_800E2850[D_800E11F6];
        func_800CEDA8(1);
        D_800F3368.parameter06 = 1;
        D_800F3368.parameter0A = 5;
        D_800F3368.depth = 32;
        if (state->timer < 33) {
            func_800CF3AC(D_801F1DE8, &color, state->timer);
            scale = func_80077CF4(state->timer << 6) + 4096;
            __asm__("" : "=r"(scale) : "0"(scale) : "memory");
            index = D_800F336C;

            four = 4;

            palette = D_800E1204[index];
            if (index == four && D_800F3428) palette += 8;
            else palette += 4;
            func_800CEE20(&position, 0, scale, scale, 64, func_80077AA4(0,palette), 1, 128, &color);
            index = D_800F336C;
            __asm__("" : "=r"(index) : "0"(index) : "$3", "memory");
            four = 4;

            palette = D_800E1204[index];
            if (index == four && D_800F3428) palette += 7;
            else palette += 3;
            func_800CEE20(&position, 0, scale, scale, 70, func_80077AA4(0,palette), 1, 128, &color);
            D_801F1F30.r = color.r; D_801F1F30.g = color.g; D_801F1F30.b = color.b;
        } else {
            *(u32 *)&D_801F1F30 = 0;
        }
        D_800F3368.parameter0A = 0;
        D_800F3368.depth = 16;
        break;
    }
    return 0;
}
