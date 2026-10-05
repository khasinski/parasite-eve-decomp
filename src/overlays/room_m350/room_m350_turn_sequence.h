#ifndef ROOM_M350_TURN_SEQUENCE_H
#define ROOM_M350_TURN_SEQUENCE_H

#include "room_m350_shared.h"

typedef struct RoomM350TurnSequenceOwner {
    unsigned int flags;
    char reserved04[20];
    RoomM350Action *action;
} RoomM350TurnSequenceOwner;

typedef struct RoomM350Instance {
    RoomM350TurnSequenceOwner *owner;
    char reserved04[10];
    unsigned char animation;
    unsigned char length;
    char reserved10[4];
    unsigned int frame;
    char reserved18[34];
    short yaw;
    char reserved3C[0x1C0];
    RoomM350Vector position;
    char reserved204[0x34];
    RoomM350TransformMatrix *transforms;
} RoomM350Instance;

PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomM350TurnSequenceOwner, action) == 0x18,
                  room_m350_turn_sequence_owner_action_offset);
PE1_STATIC_ASSERT(sizeof(RoomM350TurnSequenceOwner) == 0x1C,
                  room_m350_turn_sequence_owner_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomM350Instance, animation) == 0x0E,
                  room_m350_turn_sequence_animation_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomM350Instance, frame) == 0x14,
                  room_m350_turn_sequence_frame_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomM350Instance, yaw) == 0x3A,
                  room_m350_turn_sequence_yaw_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomM350Instance, position) == 0x1FC,
                  room_m350_turn_sequence_position_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomM350Instance, transforms) == 0x238,
                  room_m350_turn_sequence_transforms_offset);
PE1_STATIC_ASSERT(sizeof(RoomM350Instance) == 0x23C,
                  room_m350_turn_sequence_instance_size);

#endif /* ROOM_M350_TURN_SEQUENCE_H */
