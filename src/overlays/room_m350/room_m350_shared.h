#ifndef ROOM_M350_SHARED_H
#define ROOM_M350_SHARED_H

#include "pe1/gte_short_vector.h"

typedef struct RoomM350Emitter {
    int reserved[2];
    void *pool;
} RoomM350Emitter;

typedef char RoomM350Emitter_size_check[
    sizeof(RoomM350Emitter) == 0x0C ? 1 : -1];

typedef GteShortVector RoomM350Vector;

typedef struct RoomM350TransformMatrix {
    short rotation[3][3];
    int position[3];
} RoomM350TransformMatrix;

PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomM350TransformMatrix, rotation) == 0,
                  room_m350_transform_matrix_rotation_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomM350TransformMatrix, position) == 0x14,
                  room_m350_transform_matrix_position_offset);
PE1_STATIC_ASSERT(sizeof(RoomM350TransformMatrix) == 0x20,
                  room_m350_transform_matrix_size);

struct RoomM350Instance;
typedef struct RoomM350Actor {
    int reserved[2];
    struct RoomM350Instance *instance;
} RoomM350Actor;

PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomM350Actor, instance) == 8,
                  room_m350_actor_instance_offset);
PE1_STATIC_ASSERT(sizeof(RoomM350Actor) == 0x0C,
                  room_m350_actor_size);

typedef struct RoomM350SoundOwner {
    int reserved[2];
    int soundMode;
} RoomM350SoundOwner;

PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomM350SoundOwner, soundMode) == 8,
                  room_m350_sound_owner_sound_mode_offset);
PE1_STATIC_ASSERT(sizeof(RoomM350SoundOwner) == 0x0C,
                  room_m350_sound_owner_size);

typedef struct RoomM350Action {
    unsigned char state;
} RoomM350Action;

PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomM350Action, state) == 0,
                  room_m350_action_state_offset);
PE1_STATIC_ASSERT(sizeof(RoomM350Action) == 1, room_m350_action_size);

typedef struct RoomM350OverlayTransform {
    char reserved[0xF4];
    int position[3];
} RoomM350OverlayTransform;

PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomM350OverlayTransform, position) == 0xF4,
                  room_m350_overlay_transform_position_offset);
PE1_STATIC_ASSERT(sizeof(RoomM350OverlayTransform) == 0x100,
                  room_m350_overlay_transform_size);

typedef struct RoomM350SignedHalfPair {
    signed int unused : 16;
    signed int value : 16;
} RoomM350SignedHalfPair;

PE1_STATIC_ASSERT(sizeof(RoomM350SignedHalfPair) == 4,
                  room_m350_signed_half_pair_size);

typedef struct RoomM350TableShadeEntry {
    signed int size : 16;
    signed int shade : 16;
} RoomM350TableShadeEntry;

PE1_STATIC_ASSERT(sizeof(RoomM350TableShadeEntry) == 4,
                  room_m350_table_shade_entry_size);

#endif /* ROOM_M350_SHARED_H */
