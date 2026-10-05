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

#endif /* ROOM_M350_SHARED_H */
