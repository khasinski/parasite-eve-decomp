#ifndef PE1_ROOM_OBJECT_H
#define PE1_ROOM_OBJECT_H

typedef struct RoomObjectNode { int flags; } RoomObjectNode;
typedef struct RoomObjectSlot { RoomObjectNode *node; } RoomObjectSlot;
typedef struct RoomObject {
    unsigned char pad00[8];
    RoomObjectSlot *slot;
} RoomObject;

#endif
