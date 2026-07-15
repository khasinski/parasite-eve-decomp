#include "pe1/geom_state.h"

extern unsigned int D_8009D1A0;
extern unsigned int D_800B0CD8;
extern GeomState * volatile D_800B1624 __asm__("D_800B1624");
extern unsigned char D_800BCFFD;

int Scene_CheckBattleFlag(void) {
    int framePad[1];
    GeomState *state;
    GeomCtrlEntry *ctrl;
    GeomEntry *entry_base;
    GeomCtrlEntry *entry;
    unsigned int i;
    unsigned int count;
    unsigned int total;
    unsigned int j;
    unsigned int frame_count;
    unsigned char *seq;
    unsigned char *cur_seq;
    GeomEntry *geom;
    int packed;
    int accum;
    int index;
    int limit;
    int stride;
    int rem;

    asm volatile("" : : "r"(framePad));

    if (D_8009D1A0 & 0x104) {
        return 0;
    }

    if ((D_800B0CD8 & 0xC00000) == 0x800000) {
        return 0;
    }

    state = D_800B1624;
    ctrl = (GeomCtrlEntry *)((unsigned char *)state + state->ctrl_offset);
    entry_base = (GeomEntry *)((unsigned char *)state + state->entry_offset);
    total = state->entry_count;
    if (total == 0) {
        return 0;
    }

    entry = ctrl;
    for (i = 0; i < total; i++, entry++) {
        packed = *(int *)&entry->field4;
        if ((entry->head.b.flags & 2) == 0 || (entry->head.b.flags & 0x14) == 0) {
            continue;
        }
        if (*(unsigned char *)&entry->field4 != D_800BCFFD) {
            continue;
        }

        seq = (unsigned char *)entry + entry->slot_offset;
        frame_count = entry->head.packed >> 8;
        for (j = 0; j < frame_count; j++) {
            geom = &entry_base[seq[0]];
            geom->flags &= ~2;
            seq += 2;
        }

        index = (packed >> 16) * 2;
        cur_seq = (unsigned char *)entry + entry->slot_offset + index;
        geom = &entry_base[cur_seq[0]];
        geom->flags |= 2;

        if ((signed char)cur_seq[1] < 0) {
            cur_seq[1] = 0;
            entry->fieldA = 0;
            entry->head.b.flags &= ~4;
            return 0;
        }

        entry->fieldA++;
        if (entry->fieldA < (signed char)cur_seq[1]) {
            continue;
        }

        entry->fieldA = 0;
        accum = (packed >> 8) + entry->field8;
        accum = (accum << 8) | *(unsigned char *)&entry->field4;
        entry->field4 = accum;

        index = accum >> 16;
        limit = entry->head.packed >> 8;
        if (index >= limit) {
            if (entry->head.b.flags & 0x20) {
                stride = limit << 8;
                rem = (accum >> 8) % stride;
                entry->field4 = (accum & 0xFF) | (rem << 8);
            } else {
                entry->field4 = accum & 0xFF;
            }
            entry->head.b.flags &= ~4;
        } else if (index < 0) {
            if (entry->head.b.flags & 0x20) {
                stride = limit << 8;
                rem = (-(accum >> 8)) % stride;
                entry->field4 = (accum & 0xFF) | ((stride - rem) << 8);
            } else {
                entry->field4 = (accum & 0xFF) | ((limit - 1) << 16);
            }
            entry->head.b.flags &= ~4;
        }
    }

    return 0;
}
