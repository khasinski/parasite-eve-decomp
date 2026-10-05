#include "common.h"
#include "pe1/field_script_context.h"
#include "pe1/field_collision.h"
#include "pe1/gte_short_vector.h"
#include "pe1/room_spark.h"
#include "pe1/room_staged_motion.h"
#ifndef ROOMLIB_UPDATE_STAGED_MOTION_FUNC
#error "ROOMLIB_UPDATE_STAGED_MOTION_FUNC must name the room entry point"
#endif
#ifndef ROOMLIB_STAGED_MOTION_SPAWN_CALLBACK
#define ROOMLIB_STAGED_MOTION_SPAWN_CALLBACK D_80196E14
#endif
#ifndef ROOMLIB_STAGED_MOTION_SPAWN_DATA
#define ROOMLIB_STAGED_MOTION_SPAWN_DATA D_80196D6C
#endif

typedef struct RoomStagedMotionUpdateOwner {
    u8 pad00[0x4C];
    s16 hit;                          /* 0x4C: set when a record is in range */
    u8 pad4E[2];
    s32 radiusRate;                   /* 0x50: 4.12 radius growth factor */
    s32 fallRate;                     /* 0x54: fall step, in 8.8 */
} RoomStagedMotionUpdateOwner;

typedef struct RoomStagedMotionControl {
    u8 pad00;
    u8 state;                         /* 0x01: 2 once every record is spent */
} RoomStagedMotionControl;

typedef GteShortVector RoomStagedMotionPoint;

extern char ROOMLIB_STAGED_MOTION_SPAWN_CALLBACK[];
extern char ROOMLIB_STAGED_MOTION_SPAWN_DATA[];
extern s32 D_8009D248;
extern u16 D_8009D1CC;

char *func_800C2B90(void *entity, int kind, char *callback, char *data);
int func_800C6B90(void *position, int radius);

/* Advances every staged motion record: kind 1 spirals outwards and falls
 * until it leaves the floor polygon or reaches the frame-counter height,
 * spawning two child effects at that point; kinds 2.. count up to 0x14 and
 * fade their depth. Any live record within 200 units flags the owner. The
 * for loop keeps the entry compare on the counter so the retail frame keeps
 * its spare 8-byte slot and the counter wins $s6. */
void ROOMLIB_UPDATE_STAGED_MOTION_FUNC(void *entity,
                                       RoomStagedMotionControl *control,
                                       RoomStagedMotionState *state)
{
    RoomStagedMotionPoint radiusPoint;
    RoomStagedMotionPoint spawnPoint;
    RoomStagedMotionUpdateOwner *owner;
    char *spawn;
    int i;

    owner = (RoomStagedMotionUpdateOwner *)func_800C2B50();
    state->fallStep += owner->fallRate << 8;
    state->radiusStep += (state->radiusStep * owner->radiusRate) >> 12;
    for (i = 0; (unsigned int)i < state->count; i++) {
        if (state->kind[i] == 1) {
            state->phase[i] += 0x64;
            state->angle[i] += 0x14;
            state->radius[i] += state->radiusStep;
            state->record[i].x =
                ((func_80077CF4(state->angle[i]) * (state->radius[i] >> 12)) >> 12) << 16;
            state->record[i].y += state->fallStep;
            state->record[i].z =
                ((func_80077DC4(state->angle[i]) * (state->radius[i] >> 12)) >> 12) << 16;
            spawnPoint.x = (state->record[i].x >> 16) + state->x;
            spawnPoint.y = (state->record[i].y >> 16) + state->y;
            spawnPoint.z = (state->record[i].z >> 16) + state->z;
            if (Geo_PointInPoly(spawnPoint.x << 16, spawnPoint.z << 16,
                                (const PolygonVertex *)D_8009D248,
                                D_8009D1CC) == 0) {
                state->kind[i] = 0;
                state->remaining--;
                if (state->remaining == 0) {
                    control->state = 2;
                }
                spawn = func_800C2B90(entity, 6,
                                      ROOMLIB_STAGED_MOTION_SPAWN_CALLBACK,
                                      ROOMLIB_STAGED_MOTION_SPAWN_DATA);
                if (spawn != 0) {
                    *(RoomStagedMotionPoint *)spawn = spawnPoint;
                }
                spawn = func_800C2B90(entity, 6,
                                      ROOMLIB_STAGED_MOTION_SPAWN_CALLBACK,
                                      ROOMLIB_STAGED_MOTION_SPAWN_DATA);
                if (spawn != 0) {
                    *(RoomStagedMotionPoint *)spawn = spawnPoint;
                }
            }
            if (D_800942EC.count < (state->record[i].y >> 16) + state->y) {
                state->kind[i] = 2;
                spawn = func_800C2B90(entity, 6,
                                      ROOMLIB_STAGED_MOTION_SPAWN_CALLBACK,
                                      ROOMLIB_STAGED_MOTION_SPAWN_DATA);
                if (spawn != 0) {
                    *(RoomStagedMotionPoint *)spawn = spawnPoint;
                }
                spawn = func_800C2B90(entity, 6,
                                      ROOMLIB_STAGED_MOTION_SPAWN_CALLBACK,
                                      ROOMLIB_STAGED_MOTION_SPAWN_DATA);
                if (spawn != 0) {
                    *(RoomStagedMotionPoint *)spawn = spawnPoint;
                }
            }
        }
        if (state->kind[i] >= 2) {
            state->kind[i]++;
            state->depth[i] -= 8;
            if ((s16)state->depth[i] < 0) {
                state->depth[i] = 0;
            }
            if (state->kind[i] >= 0xB && state->kind[i] == 0x14) {
                state->remaining--;
                if (state->remaining == 0) {
                    control->state = 2;
                }
            }
        }
        if (state->kind[i] != 0) {
            radiusPoint.x = (state->record[i].x >> 16) + state->x;
            radiusPoint.y = (state->record[i].y >> 16) + state->y;
            radiusPoint.z = (state->record[i].z >> 16) + state->z;
            if (func_800C6B90(&radiusPoint, 0xC8)) {
                owner->hit = 1;
            }
        }
    }
}

#undef ROOMLIB_UPDATE_STAGED_MOTION_FUNC
#undef ROOMLIB_STAGED_MOTION_SPAWN_CALLBACK
#undef ROOMLIB_STAGED_MOTION_SPAWN_DATA
