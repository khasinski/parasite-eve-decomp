/* CC1_FLAGS: -fno-force-mem */
/* MASPSX_FLAGS: --expand-div */

#include "pe1/geom_state.h"
#include "pe1/game_state.h"

/* Geometry animation control: reset, update and script setup commands. */


extern GeomState * D_800B1624;

int Obj_ResetAllEntries(void) __asm__("func_800655D4");

int Obj_ResetAllEntries(void) {
    GeomState *state;
    register GeomStateAddress cursor asm("a0");
    u8 *entryBase;
    GeomEntry *renderEntries;
    unsigned int i;
    unsigned int count;
    unsigned int value100;
    unsigned int value1;
    int framePad[2];


    state = D_800B1624;
    cursor.state = D_800B1624;
    entryBase = cursor.bytes + state->ctrl_offset;
    renderEntries = (GeomEntry *)(cursor.bytes + state->entry_offset);
    count = state->entry_count;
    i = 0;

    if (count != 0) {
        value100 = 0x100;
        value1 = 1;
        cursor.bytes = entryBase;
        do {
            register unsigned int oldValue asm("v1") = (u8)cursor.ctrl->position.parts.groupFraction;
            u8 *indexedPtr = cursor.bytes + cursor.ctrl->slot_offset;
            unsigned int renderIndex;

            cursor.ctrl->step = value100;
            cursor.ctrl->elapsed = 0;
            cursor.ctrl->head.b.flags = value1;
            cursor.ctrl->position.packed = oldValue;
            renderIndex = *indexedPtr;
            renderEntries[renderIndex].flags |= 2;
            i++;
            cursor.ctrl++;
        } while (i < count);
    }


    return 0;
}



/* Historical name: advances the current group's geometry animations. */
int Scene_CheckBattleFlag(void)
{
    GeomState *state;
    GeomCtrlEntry *controls;
    GeomEntry *entries;
    unsigned int i, count;

    if (!(g_GameStateFlags & 0x104) &&
        (g_GameState.flags & 0xC00000) != 0x800000) {
        state = D_800B1624;
        controls = (GeomCtrlEntry *)((u8 *)state + state->ctrl_offset);
        entries = (GeomEntry *)((u8 *)state + state->entry_offset);
        count = state->entry_count;
        for (i = 0; i < count; i++) {
            GeomCtrlEntry *control = &controls[i];
            int position = control->position.bits.value;

            if ((control->head.b.flags & 2) &&
                (control->head.b.flags & 0x14) &&
                control->position.bits.group == g_GeomGroupSel) {
                unsigned int j;
                GeomAnimationSlot *slots =
                    (GeomAnimationSlot *)((u8 *)control + control->slot_offset);
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
                        control->position.bits.value = position + control->step;
                        next = control->position.bits.value;
                        length = control->head.packed >> 8;
                        if ((next >> 8) >= (int)length) {
                            if (control->head.b.flags & 0x20)
                                control->position.bits.value = next % (int)(length << 8);
                            else
                                control->position.bits.value = 0;
                            control->head.b.flags &= ~4;
                        } else if ((next >> 8) < 0) {
                            if (control->head.b.flags & 0x20)
                                control->position.bits.value = (int)(length << 8) -
                                    (-next % (int)(length << 8));
                            else
                                control->position.bits.value = (length - 1) << 8;
                            control->head.b.flags &= ~4;
                        }
                    }
                }
            }
        }
    }
    return 0;
}


/* Contiguous commands update the shared geometry animation control table. */


int Obj_SetEntryLimit(int arg0, int arg1)
{
  GeomCtrlEntry *entry;
  u32 positionWord;
  register u32 flags;
  u32 hi;
  int ret;
  entry = (GeomCtrlEntry *) ((((u8 *) g_GeomState) + g_GeomState->ctrl_offset) + (arg0 << 4));
  positionWord = entry->position.packed;
  positionWord &= 0xFF;
  ret = entry->head.b.flags;
  flags = ret;
  hi = arg1 << 16;
  entry->elapsed = 0;
  positionWord |= hi;
  flags |= 2;
  entry->head.b.flags = flags;
  ret = 0;
  entry->position.packed = positionWord;
  return ret;
}


int Sys_SetFlagSlot(int arg0, int arg1) {
    GeomStateAddress table, base;
    GeomCtrlEntry *slot;

    GEOM_STATE_OFFSET(table, base, ctrl_offset, 0);
    slot = table.ctrl + arg0;
    if (arg1 != 0) {
        slot->head.b.flags |= 6;
    } else {
        slot->head.b.flags &= 0xF9;
    }

    return 0;
}


#define GEOM_CTRL_ENTRY(address, base, index) \
    (GEOM_STATE_OFFSET(address, base, ctrl_offset, (index) << 4), (address).ctrl)

s16 Obj_GetEntryField6(int index) {
    GeomStateAddress address, base;

    return GEOM_CTRL_ENTRY(address, base, index)->position.parts.frame;
}

int Obj_SetEntryField8(int index, unsigned int value) {
    GeomStateAddress address, base;

    GEOM_CTRL_ENTRY(address, base, index)->step = value >> 8;
    return 0;
}

int Obj_FillEntrySlotValues(int index, u8 value)
{
  GeomStateAddress address, base;
  GeomCtrlEntry *entry;
  u8 *ptr;
  int count;
  int i;
  int framePad[2];
  i = 0;
  entry = GEOM_CTRL_ENTRY(address, base, index);
  address.word += entry->slot_offset;
  count = entry->head.packed >> 8;
  if (count != 0)
  {
    ptr = address.bytes;
    do
    {
      ptr[1] = value;
      i++;
      ptr += 2;
    }
    while (i < count);
  }
  return 0;
}

int Obj_SetEntrySlotValue(int index, int slot, u8 value) {
    GeomStateAddress address, base;
    GeomCtrlEntry *entry;
    int ret;

    entry = GEOM_CTRL_ENTRY(address, base, index);
    address.word += entry->slot_offset;
    ret = 0;
    address.bytes[slot * 2 + 1] = value;
    return ret;
}

int Obj_SetEntryFlags(int index, int bits) {
    GeomStateAddress address, base;

    GEOM_CTRL_ENTRY(address, base, index)->head.b.flags |= bits & 0x30;
    return 0;
}



/* The do { } while (0) blocks are load-bearing: their loop notes fence the
 * cc1 scheduler so the flag load/stores keep the retail order (permuter zero). */
int Menu_SetSlotEntry(int index, unsigned int limit, unsigned int slot)
{
    GeomStateAddress table, base;
    GeomCtrlEntry *entry;
    u32 positionWord;
    register u32 flags;
    u32 hi;
    u8 *slotbase;
    s32 offset;
    int neg;
    s16 f8;

    GEOM_STATE_OFFSET(table, base, ctrl_offset, index << 4);
    entry = table.ctrl;
    positionWord = entry->position.packed;
    positionWord &= 0xFF;
    offset = entry->slot_offset;
    hi = limit << 16;
    do {
        entry->elapsed = 0;
        positionWord |= hi;
        do {
            flags = entry->head.b.flags;
            table.word += offset;
            slotbase = table.bytes;
            entry->position.packed = positionWord;
        } while (0);
        neg = -1;
        flags |= 6;
        entry->head.b.flags = flags;
        slotbase[(slot << 1) + 1] = neg;
        f8 = entry->step;
        if ((f8 > 0 && slot < limit) || (f8 < 0 && limit < slot)) {
            entry->step = -(u16)entry->step;
        }
        return 0;
    } while (0);
}
