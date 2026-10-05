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

#endif /* ROOM_M350_SHARED_H */
