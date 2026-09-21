/* MASPSX_FLAGS: --expand-div */

#include "pe1/geom_state.h"
#include "pe1/game_state.h"

/* Historical name: applies texture scrolling and camera-relative parallax. */
int Scene_IsBattleMode(void)
{
    if (!(g_GameStateFlags & 0x104)) {
        int i;
        GeomState *state = D_800B1624;
        GeomScrollEntry *entries =
            (GeomScrollEntry *)((u8 *)D_800B1624 + state->entry_offset);
        int count = state->entry_count06;

        for (i = 0; i < count; i++) {
            GeomScrollEntry *entry = &entries[i];
            int position;

            if (entry->flags & 4) {
                int x, y, fracX;

                position = (entry->x * 256) | entry->fractionX.byte;
                position += entry->speedX;
                x = (position >> 8) % entry->modulusX;
                fracX = position & 255;
                position = (entry->y * 256) | entry->fractionY.byte;
                position += entry->speedY;
                y = (position >> 8) % entry->modulusY;
                entry->fractionX.word = fracX;
                entry->fractionY.word = position & 255;
                entry->x = x;
                entry->y = y;
            }
            if (entry->flags & 8) {
                /* The final fixed-point sum wraps at the target word width. */
                position = (unsigned int)(entry->baseX * 256) +
                    (D_800BCF8C.x - D_800BCF8C.originX) * entry->speedX;
                entry->x = position >> 8;
                entry->fractionX.word = position & 255;
                position = (unsigned int)(entry->baseY * 256) +
                    (D_800BCF8C.y - D_800BCF8C.originY) * entry->speedY;
                entry->y = position >> 8;
                entry->fractionY.word = position & 255;
            }
        }
        if (D_800BCF88.flags & 0x80) {
            unsigned short x = D_800BCF8C.x, y = D_800BCF8E;
            D_800BCF88.flags &= ~0x80;
            D_800BCF90 = x;
            D_800BCF92 = y;
        }
    }
    return 0;
}
