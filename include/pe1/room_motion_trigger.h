#ifndef PE1_ROOM_MOTION_TRIGGER_H
#define PE1_ROOM_MOTION_TRIGGER_H

#include "pe1/field_actor.h"

/* The grab class's instance (src/overlays/room_lib/RoomFx_GrabAttackSet.c):
 * an actor that seizes the player when it comes within reach, plays the
 * hold on the player's model and lets go when the hold ends. */
typedef struct RoomMotionTrigger {
    /* 0x00 */ u8 state;
    /* 0x01 */ u8 reserved01[2];
    /* 0x03 */ u8 flag03;
    /* 0x04 */ u8 reserved04[4];
    /* 0x08 */ FieldActor *probe_actor;
    /* 0x0C */ void (*callback)();
    /* 0x10 */ s32 *completion_state;
    /* 0x14 */ u8 reserved14[6];
    /* 0x1A */ u8 flag1A;
    /* 0x1B */ u8 reserved1B;
    /* 0x1C */ FieldActor *source_actor;
    /* 0x20 */ s32 saved_x;
    /* 0x24 */ s32 saved_z;
    /* 0x28 */ u8 activated;
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
