#ifndef ROOM_M348_SHARED_H
#define ROOM_M348_SHARED_H

#include "pe1/gte_types.h"

typedef GteVector RoomM348Vector;

typedef struct RoomM348StackFrame {
    RoomM348Vector sp10;
    s32 pad20;
    s32 sp24[3];
    RoomM348Vector sp30;
    RoomM348Vector sp40;
} RoomM348StackFrame;

PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomM348StackFrame, sp10) == 0x00,
                  room_m348_stack_frame_sp10_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomM348StackFrame, pad20) == 0x10,
                  room_m348_stack_frame_pad20_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomM348StackFrame, sp24) == 0x14,
                  room_m348_stack_frame_sp24_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomM348StackFrame, sp30) == 0x20,
                  room_m348_stack_frame_sp30_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomM348StackFrame, sp40) == 0x30,
                  room_m348_stack_frame_sp40_offset);
PE1_STATIC_ASSERT(sizeof(RoomM348StackFrame) == 0x40,
                  room_m348_stack_frame_size);

#endif /* ROOM_M348_SHARED_H */
