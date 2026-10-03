#ifndef PE1_ROOM_SOUND_SLOT_H
#define PE1_ROOM_SOUND_SLOT_H

#include "common.h"

/* The sound helper channel pointer D_800B0E64 read as a one-field record.
 * A store to an outgoing stack argument invalidates remembered in-struct
 * memory but never a scalar read through a known-constant address, so
 * reading `D_800B0E64_slot.channel` re-loads the pointer after a call's
 * argument stores the way retail does. */
typedef struct RoomSoundSlot {
    void *channel;
} RoomSoundSlot;

extern RoomSoundSlot D_800B0E64_slot __asm__("D_800B0E64");

#endif
