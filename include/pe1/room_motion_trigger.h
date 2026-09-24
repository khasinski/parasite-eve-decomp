#ifndef PE1_ROOM_MOTION_TRIGGER_H
#define PE1_ROOM_MOTION_TRIGGER_H

#include "pe1/field_actor.h"

typedef struct RoomMotionTrigger {
    u8 reserved00[8];
    FieldActor *probe_actor;
    void (*callback)(void);
    s32 *completion_state;
    u8 reserved14[8];
    FieldActor *source_actor;
    s32 saved_x;
    s32 saved_z;
    u8 activated;
} RoomMotionTrigger;

PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomMotionTrigger, probe_actor) == 0x08,
                  room_motion_trigger_probe_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomMotionTrigger, completion_state) == 0x10,
                  room_motion_trigger_completion_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomMotionTrigger, source_actor) == 0x1C,
                  room_motion_trigger_source_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomMotionTrigger, activated) == 0x28,
                  room_motion_trigger_active_offset);

#endif
