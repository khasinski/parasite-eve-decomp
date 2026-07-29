#include "pe1/room_object.h"

typedef int s32;

extern char D_80196DA4;

extern s32 FieldEng_GetStatus(RoomObject *obj);
extern void FieldEng_Register(RoomObject *obj, void *table);

s32 func_801901DC(RoomObject *obj) {
    RoomObjectNode *node;

    if (FieldEng_GetStatus(obj) == 3) {
        node = obj->slot->node;
        node->flags &= 0xBFFFFFFF;
        FieldEng_Register(obj, &D_80196DA4);
    }

    return 0;
}
