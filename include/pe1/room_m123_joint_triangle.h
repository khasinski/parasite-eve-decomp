#ifndef PE1_ROOM_M123_JOINT_TRIANGLE_H
#define PE1_ROOM_M123_JOINT_TRIANGLE_H

#include "common.h"

typedef struct RoomM123JointTriangle {
    s16 state;                    /* 0x00 */
    s16 frame;                    /* 0x02 */
    s16 position;                 /* 0x04 */
    u16 reserved06;               /* 0x06: eight-byte pool stride */
} RoomM123JointTriangle;

typedef struct RoomM123JointTriangleEmitterView {
    u16 x, y, z;                  /* 0x00 */
    u16 reserved06;               /* 0x06 */
} RoomM123JointTriangleEmitterView;

typedef union RoomM123JointTriangleRecord {
    RoomM123JointTriangle callback;
    RoomM123JointTriangleEmitterView emitter;
} RoomM123JointTriangleRecord;

PE1_STATIC_ASSERT(sizeof(RoomM123JointTriangleRecord) == 8,
                  room_m123_joint_triangle_record_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomM123JointTriangleRecord, callback.position) ==
                      PE1_OFFSETOF(RoomM123JointTriangleRecord, emitter.z),
                  room_m123_joint_triangle_position_view);

int func_8019251C(int mode, RoomM123JointTriangle *triangle);

#endif
