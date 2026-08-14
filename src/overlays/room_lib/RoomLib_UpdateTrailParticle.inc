#include "common.h"

typedef struct RoomLibTrailHistoryEntry {
    u32 word0;
    u32 word4;
} __attribute__((aligned(1), packed)) RoomLibTrailHistoryEntry;

typedef struct RoomLibTrailTrigger {
    u8 pad00;
    u8 state;
    s16 timer;
} RoomLibTrailTrigger;

typedef struct RoomLibTrailCursor {
    u8 pad00[0x76];
    u16 current;
    u16 next;
} RoomLibTrailCursor;

typedef union RoomLibTrailParticle {
    struct {
        u8 x[2];
        u8 y[2];
        u8 z[2];
        u8 pad06[0x2A];
        u8 deltaX[2];
        u8 deltaY[2];
        u8 deltaZ[2];
        u8 pad36[0x2A];
        u8 deltaYStep[4];
        u8 pad64[0x14];
        u8 trailValues[12];
    } storage;
    RoomLibTrailHistoryEntry history[6];
    struct {
        u8 pad00[0x0A];
        RoomLibTrailCursor cursor;
    } trailView;
} RoomLibTrailParticle;

extern s16 D_800942EC;

void ROOMLIB_UPDATE_TRAIL_PARTICLE_NAME(
    void *unused, RoomLibTrailTrigger *trigger, RoomLibTrailParticle *object) {
    RoomLibTrailHistoryEntry *history;
    register RoomLibTrailCursor *trail asm("$8");
    int count;

    count = 5;
    trail = &object->trailView.cursor;
    history = &object->history[5];
    do {
        *history = history[-1];
        history--;
        trail->next = trail->current;
        count--;
        trail = (RoomLibTrailCursor *)((u8 *)trail - 2);
    } while (count != 0);

    *(u16 *)object->storage.x += (s16)*(u16 *)object->storage.deltaX >> 8;
    *(u16 *)object->storage.y += (s16)*(u16 *)object->storage.deltaY >> 8;
    *(u16 *)object->storage.z += (s16)*(u16 *)object->storage.deltaZ >> 8;
    *(u16 *)object->storage.deltaY += *(s32 *)object->storage.deltaYStep;

    if (*(s16 *)object->storage.y > D_800942EC) {
        *(u16 *)object->storage.trailValues = 0;
    }
    if (trigger->timer == 60) {
        trigger->state = 2;
    }
}
