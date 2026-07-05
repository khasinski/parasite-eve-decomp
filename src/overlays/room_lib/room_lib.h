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
    RoomLinkByte *target;         /* 0x0 */
    char pad4[0xA];
    unsigned char variant;        /* 0xE */
    char padF[0x7];
    unsigned short winLo;         /* 0x16 */
    char pad18[0x2];
    unsigned short winHi;         /* 0x1A */
    char pad1C[0x4C];
    int vel[3];                   /* 0x68 */
    char pad74[0x4];
    int move[3];                  /* 0x78 */
    char pad84[0x4];
    int accel[3];                 /* 0x88 */
} RoomLink;

#define RW32(o, off) (*(int *)((char *)(o) + (off)))
#define RW16(o, off) (*(short *)((char *)(o) + (off)))
#define RW8(o, off)  (*(unsigned char *)((char *)(o) + (off)))

extern void RoomLib_HandlerA(void);

typedef struct RoomEnt {
    unsigned char state;          /* 0x0 */
    char pad1[0x2];
    unsigned char flag3;          /* 0x3 */
    char pad4[0x4];
    RoomLink *link;               /* 0x8 */
    struct RoomSub {
        void (*cb)(void);         /* 0xC */
        int *signal;              /* 0x10 */
    } sub;                        /* 0xC */
    short active;                 /* 0x14 */
    signed char t16;              /* 0x16 */
    signed char t17;
    signed char t18;
    unsigned char t19;
    unsigned char t1A;
    char pad1B[0x2B];
    short h46;                    /* 0x46 */
    char pad48[0x14];
    int w5C;                      /* 0x5C */
    int w60;
    int w64;
    char pad68[0x14];
    int w7C;                      /* 0x7C */
    int w80;                      /* 0x80 */
    char pad84[0x10];
    int w94;                      /* 0x94 */
    char pad98[0x4];
    short h9C;                    /* 0x9C */
    short h9E;
    short hA0;                    /* 0xA0 */
    char padA2[0x12];
    unsigned char bB4;            /* 0xB4 */
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

/* timers to -1, phase 3, default handler, clear signal/counters */
#define ROOMLIB_INIT_TIMERS(name) \
    int name(RoomEnt *o) { \
        o->t16 = -1; \
        o->t17 = -1; \
        o->t18 = -1; \
        o->t19 = 3; \
        o->sub.cb = RoomLib_HandlerA; \
        o->sub.signal = 0; \
        o->active = 0; \
        o->t1A = 0; \
        o->h46 = 0; \
        return 0; \
    }

/* rearm default handler when link variant matches t16 */
#define ROOMLIB_REARM_ON_MATCH(name) \
    void name(RoomEnt *o) { \
        if (o->link->variant == o->t16) { \
            o->sub.cb = RoomLib_HandlerA; \
        } \
    }

/* like ResetAndSignal but stores first; the 4 is shared via one register */
#define ROOMLIB_RESET_AND_SIGNAL_B(name) \
    int name(RoomEnt *o) { \
        struct RoomSub *s = &o->sub; \
        o->state = 4; \
        o->flag3 = 0; \
        if (o->active != 0) { \
            RoomLink *l = o->link; \
            if (l->target != 0) { \
                *l->target->state = 4; \
            } \
        } \
        if (s->signal != 0) { \
            *s->signal = 0; \
        } \
        return 0; \
    }

extern void RoomLib_HandlerB(void);
extern void RoomLib_HandlerD(void);
extern void RoomLib_FxNotify(RoomLink *l, struct RoomSub *s, int scratch);
extern void RoomLib_HandlerC(void);

/* arm handler when variant matches and t17 in (winHi, winLo] window */
#define ROOMLIB_ARM_IF_WINDOW_A(name, handler) \
    void name(RoomEnt *o) { \
        signed char c = o->t16; \
        int t; \
        unsigned short lo; \
        if (c >= 0) { \
            if (c != o->link->variant) { \
                return; \
            } \
        } \
        t = o->t17; \
        if (t >= 0) { \
            RoomLink *l = o->link; \
            lo = l->winLo; \
            if (t > l->winHi && t <= lo) { \
                o->sub.cb = handler; \
            } \
        } else { \
            o->sub.cb = handler; \
        } \
    }

#define ROOMLIB_ARM_IF_WINDOW_B(name, handler) \
    void name(RoomEnt *o) { \
        signed char c = o->t16; \
        int t; \
        unsigned short lo; \
        if (c >= 0) { \
            if (c != o->link->variant) { \
                return; \
            } \
        } \
        t = o->t17; \
        if (t >= 0) { \
            RoomLink *l = o->link; \
            lo = l->winLo; \
            if (t >= l->winHi && t <= lo) { \
                o->sub.cb = handler; \
            } \
        } else { \
            o->sub.cb = handler; \
        } \
    }

/* full entity init: flags, timers, velocities zeroed, handler armed */
#define ROOMLIB_INIT_FULL(name, handler) \
    int name(RoomEnt *o) { \
        RoomLink *l; \
        o->flag3 = 1; \
        o->t17 = -1; \
        o->t16 = -1; \
        o->t19 = 7; \
        o->w94 = 0x10000; \
        o->h9C = 0x10; \
        l = o->link; \
        o->sub.signal = 0; \
        o->active = 0; \
        o->t1A = 0; \
        o->w7C = 0; \
        o->w5C = 0; \
        o->w60 = 0; \
        o->w64 = 0; \
        o->h9E = 0; \
        o->sub.cb = handler; \
        o->w80 = 0; \
        o->hA0 = 0; \
        l->vel[0] = 0; \
        l->vel[1] = 0; \
        l->vel[2] = 0; \
        l->accel[0] = 0; \
        l->accel[1] = 0; \
        l->accel[2] = 0; \
        l->move[0] = 0; \
        l->move[1] = 0; \
        l->move[2] = 0; \
        return 0; \
    }

/* notify FX when bB4 set, then window-arm (inclusive) */
#define ROOMLIB_NOTIFY_AND_ARM_B(name, handler) \
    void name(RoomEnt *o) { \
        signed char c; \
        int t; \
        unsigned short lo; \
        if (o->bB4 != 0) { \
            RoomLib_FxNotify(o->link, &o->sub, 0x1F800000); \
        } \
        c = o->t16; \
        if (c >= 0) { \
            if (c != o->link->variant) { \
                return; \
            } \
        } \
        t = o->t17; \
        if (t >= 0) { \
            RoomLink *l = o->link; \
            lo = l->winLo; \
            if (t >= l->winHi && t <= lo) { \
                o->sub.cb = handler; \
            } \
        } else { \
            o->sub.cb = handler; \
        } \
    }

/* initializer variant B: 0x400 scale, 0x100 half, wide zero sweep */
#define ROOMLIB_INIT_B(name, handler) \
    int name(RoomEnt *o) { \
        o->flag3 = 1; \
        o->t16 = -1; \
        o->t17 = -1; \
        o->t18 = -1; \
        o->t19 = 3; \
        RW32(o, 0x9C) = 0x400; \
        RW16(o, 0xB0) = 0x100; \
        o->sub.cb = handler; \
        o->sub.signal = 0; \
        o->active = 0; \
        o->t1A = 0; \
        RW32(o, 0x94) = 0; \
        RW32(o, 0x98) = 0; \
        RW32(o, 0x8C) = 0; \
        RW32(o, 0x3C) = 0; \
        RW32(o, 0x40) = 0; \
        RW32(o, 0x44) = 0; \
        RW32(o, 0xA4) = 0; \
        RW16(o, 0xA8) = 0; \
        RW16(o, 0xAA) = 0; \
        RW16(o, 0xAC) = 0; \
        RW16(o, 0x7C) = 0; \
        RW16(o, 0x7E) = 0; \
        RW16(o, 0x80) = 0; \
        RW32(o, 0x90) = 0; \
        RW16(o, 0xAE) = 0; \
        RW32(o, 0x58) = 0; \
        RW16(o, 0xB2) = 0; \
        RW8(o, 0xB6) = 0; \
        RW8(o, 0xB7) = 0; \
        RW8(o, 0xB8) = 0; \
        return 0; \
    }

#endif
