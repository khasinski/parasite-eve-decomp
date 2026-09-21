/* CC1_FLAGS: -fno-force-mem */
/* MASPSX_FLAGS: --expand-div */

#include "pe1/geom_state.h"
#include "pe1/game_state.h"

/* Historical name: advances the current group's geometry animations. */
int Scene_CheckBattleFlag(void)
{
    GeomState *state;
    GeomAnimationControl *controls;
    GeomEntry *entries;
    unsigned int i, count;

    if (!(g_GameStateFlags & 0x104) &&
        (g_GameState.flags & 0xC00000) != 0x800000) {
        state = D_800B1624;
        controls = (GeomAnimationControl *)((u8 *)state + state->ctrl_offset);
        entries = (GeomEntry *)((u8 *)state + state->entry_offset);
        count = state->entry_count;
        for (i = 0; i < count; i++) {
            GeomAnimationControl *control = &controls[i];
            int position = control->position;

            if ((control->head.b.flags & 2) &&
                (control->head.b.flags & 0x14) &&
                control->group == g_GeomGroupSel) {
                unsigned int j;
                GeomAnimationSlot *slots =
                    (GeomAnimationSlot *)((u8 *)control + control->slotOffset);
                unsigned int frames = control->head.packed >> 8;
                GeomAnimationSlot *slot;

                for (j = 0; j < frames; j++)
                    entries[slots[j].entry].flags &= ~2;
                /* Subtraction retains retail's address-add operand order. */
                slot = slots - (-(position >> 8));
                entries[slot->entry].flags |= 2;
                if (slot->duration < 0) {
                    slot->duration = 0;
                    control->elapsed = 0;
                    control->head.b.flags &= ~4;
                    return 0;
                }
                {
                    unsigned short elapsed = control->elapsed + 1;
                    control->elapsed = elapsed;
                    if (elapsed >= slot->duration) {
                        int next;
                        unsigned int length;

                        control->elapsed = 0;
                        control->position = position + control->step;
                        next = control->position;
                        length = control->head.packed >> 8;
                        if ((next >> 8) >= (int)length) {
                            if (control->head.b.flags & 0x20)
                                control->position = next % (int)(length << 8);
                            else
                                control->position = 0;
                            control->head.b.flags &= ~4;
                        } else if ((next >> 8) < 0) {
                            if (control->head.b.flags & 0x20)
                                control->position = (int)(length << 8) -
                                    (-next % (int)(length << 8));
                            else
                                control->position = (length - 1) << 8;
                            control->head.b.flags &= ~4;
                        }
                    }
                }
            }
        }
    }
    return 0;
}
