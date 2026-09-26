#include "menu_memcard_link.h"

s32 Memcard_LinkEffect(s32 mode, MemcardLinkState *state) {
    GteShortVector unused;
    GteShortVector first;
    GteShortVector second;
    MemcardLinkOffset offset = D_801ED818;
    RenderColor color;
    MemcardLinkColor color1 = D_801ED850;
    MemcardLinkColor color2 = D_801ED854;
    register MemcardLinkOffset *offsetPtr asm("$6");
    switch (mode) {
    case 0: {
        s32 opacity;
        void *entity;
        register GteShortVector *firstPtr asm("$17");
        offsetPtr = &offset;
        __asm__("" : : "r"(offsetPtr) : "$2", "$4", "$17", "memory");
        opacity = 128; entity = D_8009D254;
        __asm__("" : "=r"(entity), "=r"(opacity) : "0"(entity), "1"(opacity) : "$17");
        firstPtr = &first;
        state->opacity = opacity; state->phase = 0; state->timer = 0;
        __asm__("" : : : "memory");
        func_800CE8F0(entity, D_801F1D86, offsetPtr, firstPtr);
        func_800CE8F0(D_8009D254, D_801F1D88, &offset, &second);
        func_800D1384(&first, &second, 1016, &color2, &color1, 128, &state->rotation, 1);
        return func_800CE560(D_800F33E0->slots, 20, 22, Memcard_PulseImage);
    }
    case 1:
        {
            s16 timer = state->timer + 1;
            void *entity = D_8009D254;

            state->timer = timer;
            __asm__("" : : : "memory");
            func_800CE8F0(entity, D_801F1D88, &offset, &unused);
        }
        if (state->timer >= 23 && state->opacity > 0) state->opacity -= 8;
        if (state->timer < 22) return 0;
        __asm__ volatile("" : : : "memory");
        return 2;
    case 2:
        if (state->opacity >= 0) {
            func_800CF3AC(D_801F1D5C, &color, state->timer);
            func_800CE8F0(D_8009D254, D_801F1D86, &offset, &first);
            func_800CE8F0(D_8009D254, D_801F1D88, &offset, &second);
            {
                RenderMatrixSlot *slot = &D_800BCFA4;
                register s32 *matrix asm("$8");
                register s32 a asm("$12");
                register s32 b asm("$13");
                register s32 c asm("$14");
                __asm__("" : "=r"(slot) : "0"(slot));
                matrix = slot->value;
                a = matrix[0]; b = matrix[1]; gte_ctc2_0(a); gte_ctc2_1(b);
                a = matrix[2]; b = matrix[3]; c = matrix[4];
                gte_ctc2_2(a); gte_ctc2_3(b); gte_ctc2_4(c);
                a = matrix[5]; b = matrix[6]; gte_ctc2_5(a);
                c = matrix[7]; gte_ctc2_6(b); gte_ctc2_7(c);
            }
            func_800D1384(&first, &second, 16, &color2, &color1,
                         state->opacity, &state->rotation, 1);
        }
        D_800F336C = 1;
        D_800F3368.tpage = D_800E2850[D_800E11E6];
        func_800CEDA8(1);
        D_800F3368.parameter06 = 0;
        D_800F3368.parameter0A = 0;
        D_800F3368.depth = 16;
        break;
    }
    return 0;
}
