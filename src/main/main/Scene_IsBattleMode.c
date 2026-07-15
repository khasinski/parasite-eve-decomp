#include "pe1/geom_state.h"

extern int D_8009D1A0;
extern GeomState * volatile D_800B1624 __asm__("D_800B1624");
extern s32 D_800BCF88;
extern s16 D_800BCF8C;
extern s16 D_800BCF8E;
extern s16 D_800BCF90;
extern s16 D_800BCF92;
extern s16 D_800BD028;
extern s16 D_800BD02A;

int Scene_IsBattleMode(void) {
    int framePad[2];
    int i;
    u16 count;
    GeomState *state;
    GeomEntry *entry;
    s32 accum;
    s32 coord;

    asm volatile("" : : "r"(framePad));

    if (D_8009D1A0 & 0x104) {
        asm volatile("" : : : "memory");
        return 0;
    }

    i = 0;
    state = D_800B1624;
    entry = (GeomEntry *)((u8 *)D_800B1624 + state->entry_offset);
    count = state->entry_count06;

    if (count != 0) {
        do {
            if (entry->flags & 4) {
                accum = ((entry->scr_x << 8) | entry->field20) + entry->field1C;
                coord = accum >> 8;
                entry->field20 = accum & 0xFF;
                entry->scr_x = coord % entry->anim_mod_x;

                accum = ((entry->scr_y << 8) | entry->field22) + entry->field1E;
                coord = accum >> 8;
                entry->field22 = accum & 0xFF;
                entry->scr_y = coord % entry->anim_mod_y;
            }

            if (entry->flags & 8) {
                accum = (entry->base_x << 8) + ((D_800BCF8C - D_800BD028) * entry->field1C);
                entry->scr_x = accum >> 8;
                entry->field20 = accum & 0xFF;

                accum = (entry->base_y << 8) + ((D_800BCF8E - D_800BD02A) * entry->field1E);
                entry->scr_y = accum >> 8;
                entry->field22 = accum & 0xFF;
            }

            i++;
            entry++;
        } while (i < count);
    }

    if (D_800BCF88 & 0x80) {
        D_800BCF88 &= ~0x80;
        D_800BCF90 = D_800BCF8C;
        D_800BCF92 = D_800BCF8E;
    }

    asm volatile("" : : : "memory");
    return 0;
}
