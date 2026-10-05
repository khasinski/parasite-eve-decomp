#ifndef ROOM_SHARED_GLOBAL_STATE_H
#define ROOM_SHARED_GLOBAL_STATE_H

#include "common.h"

typedef struct RoomSharedGlobalState {
    char pad0[0x2A];
    s16 field2A;
} RoomSharedGlobalState;

PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomSharedGlobalState, field2A) == 0x2A,
                  room_shared_global_state_field2a_offset);
PE1_STATIC_ASSERT(sizeof(RoomSharedGlobalState) == 0x2C,
                  room_shared_global_state_size);

#endif /* ROOM_SHARED_GLOBAL_STATE_H */
