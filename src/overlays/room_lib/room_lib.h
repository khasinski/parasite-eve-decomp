/* Shared room-script library. Every room links its own copy; instance
 * functions are stamped per room/offset with these body macros. */
#ifndef ROOM_LIB_H
#define ROOM_LIB_H

typedef struct RoomObj {
    char pad0[0xC];
    void (*callback)(void);
} RoomObj;

#define ROOMLIB_INVOKE_CALLBACK_C(name) \
    int name(RoomObj *obj) { \
        obj->callback(); \
        return 0; \
    }

#endif
