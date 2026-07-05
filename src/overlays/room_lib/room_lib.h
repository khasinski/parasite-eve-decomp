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

typedef struct RoomLinkByte {
    char pad18[0x18];
    unsigned char *state;
} RoomLinkByte;

typedef struct RoomLink {
    RoomLinkByte *target;
} RoomLink;

typedef struct RoomEnt {
    unsigned char state;          /* 0x0 */
    char pad1[0x2];
    unsigned char flag3;          /* 0x3 */
    char pad4[0x4];
    RoomLink *link;               /* 0x8 */
    struct RoomSub {
        int pad0;
        int *signal;              /* 0x10 */
    } sub;                        /* 0xC */
    short active;                 /* 0x14 */
} RoomEnt;

/* state=4, clear flag3, notify link target, clear signal word */
#define ROOMLIB_RESET_AND_SIGNAL(name) \
    int name(RoomEnt *o) { \
        struct RoomSub *s = &o->sub; \
        if (o->active != 0) { \
            RoomLink *l = o->link; \
            if (l->target != 0) { \
                *l->target->state = 4; \
            } \
        } \
        o->flag3 = 0; \
        o->state = 4; \
        if (s->signal != 0) { \
            *s->signal = 0; \
        } \
        return 0; \
    }

/* state=4 and clear the signal word */
#define ROOMLIB_SET4_CLEAR_SIGNAL(name) \
    int name(RoomEnt *o) { \
        int *p = o->sub.signal; \
        o->state = 4; \
        if (p != 0) { \
            *p = 0; \
        } \
        return 0; \
    }

#endif
