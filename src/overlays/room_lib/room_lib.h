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

#define RW32(o, off) (*(int *)((char *)(o) + (off)))
#define RW16(o, off) (*(short *)((char *)(o) + (off)))
#define RW8(o, off)  (*(unsigned char *)((char *)(o) + (off)))
#define RWU16(o, off) (*(unsigned short *)((char *)(o) + (off)))
#define RVW16(o, off) (*(volatile short *)((char *)(o) + (off)))

extern void RoomLib_HandlerA();
extern void RoomLib_HandlerB(void);
extern void RoomLib_HandlerC(void);
extern void RoomLib_HandlerD(void);
extern void RoomLib_HandlerE(void);
extern void RoomLib_HandlerF();
extern int FieldEng_VecToAngle(int *vec, int *ref);
extern int FieldEng_TurnToward(short cur, short target, short rate);
extern char *RoomMain_ActorPtr;
extern int RoomMain_RotTable[];
extern unsigned int FieldEng_GetStatus(void);
extern void FieldEng_Spawn6(int a, int b, int c, int d, int e, int f);
extern void FieldEng_Register(void *o, void *table);
extern int func_800C251C(void *o, void *table);
extern int func_800C2758(void *o, void *tableA, void *tableB);
extern void **FieldEng_GetSlot(void);

typedef struct RoomRenderNode {
    int flags;                    /* 0x00: 0x3F000000 owner bits, 0xC0FFFFFF mask dance */
    char pad04[0x14];
    unsigned char *state;         /* 0x18: byte poked with entity state (4) */
} RoomRenderNode;

/* The actor/object record shared with the field engine (>= 0x254 bytes).
 * Positions are 16.16 fixed point; offsets mined from 90 canonical shapes. */
typedef struct RoomLink {
    RoomRenderNode *target;       /* 0x00 */
    char pad04[0xA];
    unsigned char variant;        /* 0x0E: matched against RoomEnt.t16 */
    char pad0F[0x7];
    unsigned short winLo;         /* 0x16: t17 window upper bound */
    char pad18[0x2];
    unsigned short winHi;         /* 0x1A: t17 window lower bound */
    char pad1C[0xA];
    unsigned short h26;           /* 0x26 */
    int pos[3];                   /* 0x28: 16.16 x/y/z (hi16 read as s16) */
    char pad34[0x6];
    unsigned short h3A;           /* 0x3A */
    char pad3C[0x2C];
    int vel[3];                   /* 0x68 */
    char pad74[0x4];
    int move[3];                  /* 0x78 */
    char pad84[0x4];
    int accel[3];                 /* 0x88 */
    char pad94[0x4];
    void *node98;                 /* 0x98 */
    char pad9C[0xF0];
    struct RoomLink *link18C;     /* 0x18C: nested link record */
    char pad190[0x6C];
    int w1FC;                     /* 0x1FC */
    int w200;
    int w204;
    char pad208[0x30];
    void *p238;                   /* 0x238 */
    char pad23C[0x14];
    unsigned short h250;          /* 0x250 */
} RoomLink;

typedef RoomRenderNode RoomLinkByte;  /* legacy alias */

/* Script-entity record (~0xC4 bytes), one per room script object. */
typedef struct RoomEnt {
    unsigned char state;          /* 0x00: 4 = closed */
    char pad1[0x2];
    unsigned char flag3;          /* 0x03 */
    int w04;                      /* 0x04 */
    RoomLink *link;               /* 0x08 */
    struct RoomSub {
        void (*cb)(void);         /* 0x0C: armed FX handler */
        int *signal;              /* 0x10: completion word (0/1/2) */
    } sub;
    short active;                 /* 0x14 */
    signed char t16;              /* 0x16: variant gate */
    signed char t17;              /* 0x17: window gate */
    signed char t18;
    unsigned char t19;            /* 0x19: phase (3/7) */
    unsigned char t1A;
    char pad1B[0x1];
    short mat[9];                 /* 0x1C: s16 3x3 rotation (MATRIX.m) */
    short rot[3];                 /* 0x2E: euler angles */
    short h34;                    /* 0x34 */
    char pad36[0x4];
    short heading;                /* 0x3A */
    int pos[2];                   /* 0x3C, 0x40 */
    short h44;                    /* 0x44: word-view via RW32 in some shapes */
    short h46;                    /* 0x46 */
    short h48;
    char pad4A[0x12];
    int w5C;                      /* 0x5C */
    int w60;
    int w64;
    char pad68[0x14];
    int w7C;                      /* 0x7C */
    int w80;                      /* 0x80 */
    char pad84[0x10];
    int w94;                      /* 0x94 */
    char pad98[0x4];
    short h9C;                    /* 0x9C: also written as word with 0x9E */
    short h9E;
    short hA0;                    /* 0xA0 */
    char padA2[0x12];
    unsigned char bB4;            /* 0xB4: FX-notify gate */
    char padB5[0x1];
    unsigned char bB6;            /* 0xB6 */
    unsigned char bB7;
    unsigned char bB8;
    char padB9[0x3];
    unsigned short hBC[4];        /* 0xBC..0xC2 */
} RoomEnt;

extern void RoomLib_FxNotify(RoomLink *l, struct RoomSub *s, int scratch);
extern void RoomLib_FxNotify2(RoomLink *l, struct RoomSub *s);

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

#define ROOMLIB_HANDLER_F_VARIANT(name, clearFn, actorGlobal) \
    void name(RoomEnt *o) { \
        register RoomLink *actor asm("$3") = actorGlobal; \
        register int flags asm("$4"); \
        register int check asm("$2"); \
        register int masked asm("$2"); \
        if (actor->variant >= 4U) { \
            clearFn(o); \
            return; \
        } \
        flags = RW32(actor, 0x98); \
        check = flags & 0xC0000; \
        if (check != 0) { \
            masked = flags & 0xFFF3FFFF; \
            RW32(actor, 0x98) = masked; \
            clearFn(o); \
            return; \
        } \
        asm volatile("lui $3,0x1F80\n\t" \
                     "sh $0,0($3)\n\t" \
                     "lui $1,0x1F80\n\t" \
                     "sh $0,2($1)\n\t" \
                     "lw $2,0x3C($16)\n\t" \
                     "nop\n\t" \
                     "sra $2,$2,12\n\t" \
                     "lui $1,0x1F80\n\t" \
                     "sh $2,4($1)\n\t" \
                     "addiu $2,$16,0x1C\n\t" \
                     "lw $12,0($2)\n\t" \
                     "lw $13,4($2)\n\t" \
                     "ctc2 $12,$0\n\t" \
                     "ctc2 $13,$1\n\t" \
                     "lw $12,8($2)\n\t" \
                     "lw $13,12($2)\n\t" \
                     "lw $14,16($2)\n\t" \
                     "ctc2 $12,$2\n\t" \
                     "ctc2 $13,$3\n\t" \
                     "ctc2 $14,$4\n\t" \
                     "lwc2 $0,0($3)\n\t" \
                     "lwc2 $1,4($3)\n\t" \
                     "nop\n\t" \
                     "nop\n\t" \
                     ".word 0x4A486012\n\t" \
                     "lui $2,0x1F80\n\t" \
                     "ori $2,$2,8\n\t" \
                     "swc2 $25,0($2)\n\t" \
                     "swc2 $26,4($2)\n\t" \
                     "swc2 $27,8($2)\n\t" \
                     "lui $4,%%hi(" #actorGlobal ")\n\t" \
                     "lw $4,%%lo(" #actorGlobal ")($4)\n\t" \
                     "lw $2,0($2)\n\t" \
                     "lw $3,0x28($4)\n\t" \
                     "sll $2,$2,12\n\t" \
                     "addu $3,$3,$2\n\t" \
                     "sw $3,0x28($4)\n\t" \
                     "lui $2,0x1F80\n\t" \
                     "lw $2,0x10($2)\n\t" \
                     "lw $3,0x30($4)\n\t" \
                     "sll $2,$2,12\n\t" \
                     "addu $3,$3,$2\n\t" \
                     "sw $3,0x30($4)" \
                     : \
                     : \
                     : "$1", "$2", "$3", "$4", "$12", "$13", "$14", "memory"); \
        o->pos[0] -= o->pos[1]; \
        if (o->pos[0] < 0) { \
            clearFn(o); \
        } \
        if (o->h46 != 0) { \
            actorGlobal->h3A = FieldEng_TurnToward(actorGlobal->h3A, o->h48, o->h46); \
        } \
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

/* argument parser paired with InitTimers/RearmOnMatch */
#define ROOMLIB_SCHED_BARRIER() __asm__ volatile("")

#define ROOMLIB_INIT_TIMER_ARGS(name, rearmFn, jumpWord) \
    int name(RoomEnt *o, int a1, unsigned int kind, int a3, int arg4, int arg5) { \
        int value = 10; \
        register int stack0 asm("t0") = arg4; \
        register int stack1 asm("v1") = arg5; \
        __asm__ volatile("" : "=r"(stack0), "=r"(stack1) : "0"(stack0), "1"(stack1)); \
        if (kind == value) { \
            goto case_10; \
        } \
        ROOMLIB_SCHED_BARRIER(); \
        if (kind >= 11U) { \
            goto high_kind; \
        } \
        value = 4; \
        if (kind == value) { \
            goto case_4; \
        } \
        goto common; \
high_kind: \
        value = 25; \
        if (kind == value) { \
            goto case_25; \
        } \
        value = 28; \
        if (kind == value) { \
            goto case_28; \
        } \
        goto common; \
case_25: \
        value = 1; \
        if (a1 != value) { \
            goto common; \
        } \
        o->sub.signal = (int *)a3; \
        __asm__ volatile( \
            ".word " #jumpWord "\n" \
            ".word 0xACE50000"); \
case_4: \
        o->pos[0] = a3; \
        o->pos[1] = stack0; \
        o->h44 = stack1; \
        goto common; \
case_28: \
        RW16(o, 0x48) = a3; \
        o->h46 = stack0; \
        goto common; \
case_10: \
        value = (int)rearmFn; \
        o->t16 = a3; \
        RW32(o, 0xC) = value; \
common: \
        ROOMLIB_SCHED_BARRIER(); \
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

extern char RoomLib_TableA[];
extern char RoomLib_TableB[];

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

/* notify FX (2-arg) when h80 set, then window-arm (inclusive) */
#define ROOMLIB_NOTIFY2_AND_ARM_B(name, handler) \
    void name(RoomEnt *o) { \
        struct RoomSub *s = &o->sub; \
        signed char c; \
        int t; \
        unsigned short lo; \
        if (RW16(o, 0x80) != 0) { \
            RoomLib_FxNotify2(o->link, s); \
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

/* initializer variant C: 0x10000 at 0x8C, zero sweep, link triads */
#define ROOMLIB_INIT_C(name, handler) \
    int name(RoomEnt *o) { \
        RoomLink *l; \
        o->flag3 = 1; \
        o->t17 = -1; \
        o->t16 = -1; \
        o->t19 = 7; \
        RW32(o, 0x8C) = 0x10000; \
        l = o->link; \
        o->sub.signal = 0; \
        o->active = 0; \
        o->t1A = 0; \
        RW32(o, 0x74) = 0; \
        RW32(o, 0x78) = 0; \
        RW32(o, 0x7C) = 0; \
        RW32(o, 0x80) = 0; \
        RW16(o, 0x92) = 0; \
        RW32(o, 0x4C) = 0; \
        RW32(o, 0x50) = 0; \
        RW32(o, 0x54) = 0; \
        RW16(o, 0x94) = 0; \
        RW32(o, 0x6C) = 0; \
        o->sub.cb = handler; \
        RW32(o, 0x70) = 0; \
        RW16(o, 0x96) = 0; \
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

/* steer entity heading toward the link's tracked point */
#define ROOMLIB_STEER_TOWARD(name) \
    void name(char *ent, char *rec) { \
        if (*(short *)(rec + 0x6E) > 0) { \
            int *dst = (int *)(rec + 0x50); \
            char *v = *(char **)(rec + 0x64); \
            if (v != 0) { \
                dst[0] = *(int *)(v + 0x28); \
                dst[2] = *(int *)(v + 0x30); \
            } \
            *(short *)(ent + 0x3A) = FieldEng_TurnToward( \
                *(short *)(ent + 0x3A), \
                (short)FieldEng_VecToAngle(dst, (int *)(ent + 0x28)), \
                *(short *)(rec + 0x6E)); \
        } \
    }

/* initializer variant D: 0x10000 at 0x74, short/byte sweep */
#define ROOMLIB_INIT_D(name, handler) \
    int name(RoomEnt *o) { \
        o->flag3 = 1; \
        o->t16 = -1; \
        o->t17 = -1; \
        o->t18 = -1; \
        o->t19 = 3; \
        RW32(o, 0x74) = 0x10000; \
        o->sub.cb = handler; \
        o->sub.signal = 0; \
        o->active = 0; \
        o->t1A = 0; \
        RW32(o, 0x4C) = 0; \
        RW32(o, 0x50) = 0; \
        RW32(o, 0x54) = 0; \
        RW32(o, 0x6C) = 0; \
        RW32(o, 0x70) = 0; \
        RW16(o, 0x7A) = 0; \
        RW16(o, 0x80) = 0; \
        RW16(o, 0x82) = 0; \
        RW16(o, 0x84) = 0; \
        RW16(o, 0x86) = 0; \
        RW16(o, 0x88) = 0; \
        RW8(o, 0x90) = 0; \
        RW8(o, 0x91) = 0; \
        return 0; \
    }

/* face the actor: clear actor bits, arm handler, aim h44/h48, load rot */
#define ROOMLIB_FACE_ACTOR_WITH_GLOBALS(name, handler, actorGlobal, rotTable) \
    void name(RoomEnt *o) { \
        char *g = actorGlobal; \
        if (RW8(g, 0xE) < 4) { \
            int *sig; \
            RW32(g, 0x98) &= 0xFFF3FFFF; \
            sig = o->sub.signal; \
            o->sub.cb = handler; \
            if (sig != 0) { \
                *sig = 2; \
            } \
            if (RW16(o, 0x44) == -1) { \
                RW16(o, 0x44) = FieldEng_VecToAngle( \
                    (int *)((char *)o->link + 0x28), \
                    (int *)(actorGlobal + 0x28)); \
            } \
            if (RW16(o, 0x48) == -1) { \
                RW16(o, 0x48) = RWU16(o, 0x44) + 0x800; \
            } \
            { \
                int *e = (int *)((char *)rotTable \
                                 + ((RWU16(o, 0x44) & 0xFFF) << 2)); \
                int hi = *(short *)((char *)e + 2); \
                int lo; \
                int savedHi; \
                RVW16(o, 0x1E) = 0; \
                RVW16(o, 0x1C) = hi; \
                lo = *e; \
                savedHi = RWU16(o, 0x1C); \
                RVW16(o, 0x22) = 0; \
                RVW16(o, 0x26) = 0; \
                RVW16(o, 0x2A) = 0; \
                RVW16(o, 0x20) = lo; \
                RVW16(o, 0x24) = 0x1000; \
                RVW16(o, 0x2C) = savedHi; \
                RVW16(o, 0x28) = -RWU16(o, 0x20); \
            } \
        } \
    }

#define ROOMLIB_FACE_ACTOR(name, handler) \
    ROOMLIB_FACE_ACTOR_WITH_GLOBALS(name, handler, RoomMain_ActorPtr, RoomMain_RotTable)

/* six-argument passthrough to the field engine spawn */
#define ROOMLIB_SPAWN6(name) \
    int name(int a, int b, int c, int d, int e, int f) { \
        FieldEng_Spawn6(a, b, c, d, e, f); \
        return 0; \
    }

/* close entity: state 4 here and on the link target, clear high flags */
#define ROOMLIB_CLOSE_TARGET(name) \
    int name(RoomEnt *o) { \
        o->state = 4; \
        if (FieldEng_GetStatus() >= 2) { \
            RoomLinkByte *tgt = o->link->target; \
            *(int *)tgt &= 0xC0FFFFFF; \
            *o->link->target->state = 4; \
        } \
        return 0; \
    }

/* register this entity's table with the field engine when active */
#define ROOMLIB_REGISTER_TABLE(name, table) \
    int name(void *o) { \
        if (FieldEng_GetStatus() >= 2) { \
            FieldEng_Register(o, table); \
        } \
        return 0; \
    }

/* register paired room tables, closing this target when either engine call fails */
#define ROOMLIB_REGISTER_PAIR_OR_CLOSE(name, tableA, tableB, tableC, closeFn) \
    int name(RoomEnt *o) { \
        int result; \
        if (FieldEng_GetStatus() < 2) { \
            goto fail; \
        } else { \
            result = func_800C251C(o, tableC); \
            result |= func_800C2758(o, tableA, tableB); \
        } \
        goto done; \
fail: \
        __asm__ volatile(""); \
        result = -1; \
done: \
        if (result == -1) { \
            closeFn(o); \
        } \
        return 0; \
    }

/* same table registration pattern, but only while the field engine is in status 3 */
#define ROOMLIB_REGISTER_PAIR_STATUS3_OR_CLOSE(name, tableA, tableB, tableC, closeFn) \
    int name(RoomEnt *o) { \
        int result; \
        if (FieldEng_GetStatus() != 3) { \
            goto fail; \
        } else { \
            result = func_800C251C(o, tableC); \
            result |= func_800C2758(o, tableA, tableB); \
        } \
        goto done; \
fail: \
        __asm__ volatile(""); \
        result = -1; \
done: \
        if (result == -1) { \
            closeFn(o); \
        } \
        return 0; \
    }

/* register unless a low-variant linked target has its room-local AC gate clear */
#define ROOMLIB_REGISTER_PAIR_UNLESS_TARGET_AC_CLEAR(name, tableA, tableB, tableC, closeFn) \
    int name(RoomEnt *o) { \
        int result = 0; \
        if (FieldEng_GetStatus() < 2) { \
            goto fail; \
        } \
        if (o->link->variant < 2) { \
            if (*((unsigned char *)o->link->target + 0xAC) == 0) { \
                goto done; \
            } \
        } \
        result = func_800C251C(o, tableC); \
        result |= func_800C2758(o, tableA, tableB); \
        goto done2; \
fail: \
        result = -1; \
done2: \
        __asm__ volatile(""); \
done: \
        if (result == -1) { \
            closeFn(o); \
        } \
        return 0; \
    }

/* plant the room table pointer into the engine slot */
#define ROOMLIB_PLANT_TABLE(name, table) \
    int name(void) { \
        *FieldEng_GetSlot() = table; \
        return 0; \
    }

/* stash three args into room slots; returns the first slot (asm reuse) */
#define ROOMLIB_SET_ARGS2(name, sA, sB) \
    int *name(int a0, int a1, int a2) { \
        int *p = &sA; \
        *p = a1; \
        sB = a2; \
        return p; \
    }

#define ROOMLIB_SET_ARGS3(name, sA, sB, sC) \
    int *name(int a0, int a1, int a2, int a3) { \
        int *p = &sA; \
        *p = a1; \
        sB = a2; \
        sC = a3; \
        return p; \
    }

/* stash one arg into a room slot; returns the slot */
#define ROOMLIB_SET_ARG1(name, sA) \
    int *name(int a0, int a1) { \
        int *p = &sA; \
        *p = a1; \
        return p; \
    }

/* --- m017 family: room-local model controller (m017/m018/m021/m045/
 * m102/m151/m319 share one prologue library block at 0x80100014+). --- */

struct FieldActorNode;

typedef struct M17Model {
    unsigned char b00;
    unsigned char b01;
    short h02;
    int w04;
    unsigned char *obj;                    /* 0x08: owning object; geometry at +0x1B4 */
    struct FieldActorNode *actorNode; /* 0x0C */
    short h10;                    /* 0x10 */
    short h12;                    /* 0x12 */
    short h14;                    /* 0x14 */
    short h16;                    /* 0x16 */
    unsigned char bInit;                   /* 0x18 */
    unsigned char bActive;                 /* 0x19 */
} M17Model;

typedef struct M17Ent {
    unsigned char pad00[0x28];
    short h28;                    /* 0x28 */
    unsigned char pad2A[0x36];
    int w60;                    /* 0x60 */
    unsigned char pad64[0x38];
    unsigned short h9C;                    /* 0x9C: flags */
    unsigned char pad9E[0x1E];
    unsigned short hBC;                    /* 0xBC */
    unsigned short hBE;                    /* 0xBE */
    unsigned short hC0;                    /* 0xC0 */
    unsigned short hC2;                    /* 0xC2 */
} M17Ent;

extern void RoomFx_ModelBind(unsigned char *geom, int actor, int a, int b, int c, int d);
extern void RoomFx_ModelUpdate(unsigned char *geom);
extern void RoomFx_ModelDraw(unsigned char *geom);
extern void Render_TransformVertices(unsigned char *geom);
extern void Render_TransformMorphVertices(unsigned char *geom, void *dst);
extern void *g_GeomVramPacketDst;

#define ROOMLIB_MDL_RESET(name) \
    int name(M17Model *m) { \
        m->actorNode = 0; \
        m->h12 = 0; \
        m->h10 = 0; \
        m->h16 = 0; \
        m->h14 = 0; \
        m->bInit = 0; \
        m->bActive = 0; \
        return 0; \
    }

#define ROOMLIB_MDL_KILL(name) \
    int name(unsigned char *state) { \
        *state = 4; \
        return 0; \
    }

#define ROOMLIB_ENT_SETUP5(name) \
    void name(M17Ent *e, int arg1, unsigned short a, unsigned short b, unsigned short c, unsigned short d) { \
        e->h28 = 5; \
        e->w60 = arg1; \
        e->hBC = a; \
        e->hBE = b; \
        e->h9C |= 0x1000; \
        e->hC0 = c; \
        e->hC2 = d; \
    }

#define ROOMLIB_MDL_TICK(name) \
    int name(M17Model *m) { \
        if (m->bInit == 0) { \
            RoomFx_ModelBind(m->obj + 0x1B4, (int)m->actorNode, m->h10, m->h12, \
                             m->h14, m->h16); \
            m->bInit = 1; \
        } \
        if (m->bActive != 0) { \
            RoomFx_ModelUpdate(m->obj + 0x1B4); \
            Render_TransformVertices(m->obj + 0x1B4); \
            Render_TransformMorphVertices(m->obj + 0x1B4, g_GeomVramPacketDst); \
            RoomFx_ModelDraw(m->obj + 0x1B4); \
        } \
        return 0; \
    }

typedef struct FieldActorNode {
    int w00;
    struct FieldActorNode *next;  /* 0x04 */
    int w08;
    unsigned char b0C;            /* 0x0C: kind matched against arg */
    unsigned char b0D;            /* 0x0D: sub-kind */
    char pad0E[0x8A];
    int w98;                      /* 0x98: 0x10 = busy */
} FieldActorNode;

extern FieldActorNode *g_FieldActorListHead;

#define ROOMLIB_MDL_SELECT(name) \
    int name(M17Model *m, int cancel, int mode, int a, int b) { \
        FieldActorNode **p = &m->actorNode; \
        if (mode == 1) goto win_a; \
        if (mode == 0) goto scan; \
        if (mode == 2) goto win_b; \
        if (mode == 3) goto flag; \
        return -6; \
    scan: \
        if (cancel != 0) { \
            goto done; \
        } \
        *p = g_FieldActorListHead; \
        if (*p != 0) { \
            do { \
                FieldActorNode *n = *p; \
                if (n->b0C == a && n->b0D == b && !(n->w98 & 0x10)) { \
                    goto done; \
                } \
                *p = (*p)->next; \
            } while (*p != 0); \
        } \
        goto done; \
    win_a: \
        m->h10 = a; \
        m->h12 = b; \
        goto done; \
    win_b: \
        m->h14 = a; \
        m->h16 = b; \
        goto done; \
    flag: \
        m->bActive = a; \
    done: \
        return 0; \
    }

typedef struct RoomSlotRec {
    short h0;
    short h2;                     /* stamped with the global frame counter */
    short h4;
    short pad6;
    int w8;
    int wC;
} RoomSlotRec;

extern unsigned short g_FrameCount16;

#define ROOMLIB_SLOT_SET(name, table) \
    RoomSlotRec *name(int mode, int idx, int a, int b) { \
        RoomSlotRec *e = &table[idx]; \
        if (mode == 1) { \
            e->w8 = a; \
            e->wC = b; \
        } else { \
            *(volatile short *)&e->h0 = a; \
            e->h2 = g_FrameCount16; \
            e->h4 = b; \
        } \
        return e; \
    }

typedef struct RoomBlob8 {
    char b[8];
} RoomBlob8;

typedef struct RoomMsgSub {
    short h0;
} RoomMsgSub;

typedef struct RoomMsg {
    short h0;
    short h2;
    short h4;
    short h6;
    RoomMsgSub sub;               /* 0x08 */
} RoomMsg;

typedef struct RoomQRec {
    short h0;
    short h2;
    short h4;
    short h6;
    char sub[8];                  /* 0x08 */
    short h10;                    /* 0x10 */
    short h12;                    /* 0x12 */
} RoomQRec;

typedef struct RoomNodeB {
    char pad[0x18];
    unsigned char *state;         /* 0x18 */
} RoomNodeB;

typedef struct RoomChanCtx {
    int w0;
    int w4;
    RoomNodeB **w8;               /* 0x08 */
} RoomChanCtx;

typedef struct RoomDlgState {
    char pad[0xD];
    unsigned char bD;             /* 0x0D */
    char padE[0x4];
    short h12;                    /* 0x12 */
} RoomDlgState;

extern RoomChanCtx *D_800F32D0;
extern RoomChanCtx *D_800F33E0;
extern RoomDlgState *D_800E2368;
extern int D_800E27EC;
extern short D_800F3372;
extern short D_800F3374;
extern void *RoomMain_ActorPtr2;
extern int func_800CE8F0();
extern int func_800CE9D4();
extern int func_800CE870();
extern int func_800CFAA8();
extern int func_800CE560();
extern RoomQRec *func_800CE610();
extern int func_800CFB7C();
extern int func_800D3FD8();
extern int func_800D3F64();

#define ROOMLIB_MSG_DISPATCH(name, rect, blob) \
    int name(int mode, RoomMsg *msg, short *pitch) { \
        RoomBlob8 tmp = rect; \
        switch (mode) { \
        case 0: \
            func_800CE8F0(D_800F32D0->w8, 0x13, &tmp, msg); \
            if (D_800E2368->h12 == 0) goto send_pos; \
            if (D_800E2368->h12 == 1) goto at_actor; \
            goto after; \
        send_pos: \
            func_800CE9D4(D_800F32D0->w8, 0x13, &msg->sub); \
            goto after; \
        at_actor: \
            { \
                short vec[4]; \
                func_800CE870(RoomMain_ActorPtr2, 0, vec); \
                func_800CFAA8(msg, vec, &msg->sub); \
                msg->sub.h0 = 0x180; \
            } \
        after: \
            if (D_800E2368->bD != 0) { \
                RoomNodeB **q = D_800F32D0->w8; \
                if (q != 0 && *q != 0) { \
                    unsigned char *st = (*q)->state; \
                    if (*st == 1) { \
                        *st = 2; \
                    } \
                } \
            } \
            return func_800CE560(D_800F33E0->w8, 0x14, 0x18, &blob); \
        case 1: \
            if (D_800E27EC == mode) { \
                RoomQRec *p = func_800CE610(D_800F33E0->w8); \
                if (p != 0) { \
                    p->h0 = msg->h0; \
                    p->h2 = msg->h2; \
                    p->h4 = msg->h4; \
                    func_800CFB7C(&msg->sub, *pitch, p->sub); \
                    p->h10 = 0; \
                    p->h12 = 0; \
                } \
                p = func_800CE610(D_800F33E0->w8); \
                if (p != 0) { \
                    p->h0 = msg->h0; \
                    p->h2 = msg->h2; \
                    p->h4 = msg->h4; \
                    p->h10 = 3; \
                    p->h12 = 0; \
                } \
                func_800D3F64(0x586, func_800D3FD8()); \
            } \
            if (D_800E27EC < 2) goto ret0; \
            return 2; \
        case 2: \
            D_800F3372 = 0; \
            D_800F3374 = 8; \
            goto ret0; \
        default: \
        ret0: \
            return 0; \
        } \
    }

/* m063/m083 pair: per-state dispatcher + window-armer via the nested link */
extern int func_800DFB78();

#define ROOMLIB_STATE_DISPATCH(name, tickFn) \
    int name(RoomEnt *o) { \
        switch (func_800DFB78()) { \
        case 0: \
            ((void (*)(RoomEnt *))o->sub.cb)(o); \
            return 0; \
        case 1: \
            tickFn(o); \
        case 2: \
            return 0; \
        } \
        return 0; \
    }

#define ROOMLIB_STATE_DISPATCH_VARIANT2(name, tickFn) \
    int name(RoomEnt *o) { \
        switch (func_800DFB78()) { \
        case 0: \
            if (o->link->variant < 2) { \
                return 0; \
            } \
            ((void (*)(RoomEnt *))o->sub.cb)(o); \
            return 0; \
        case 1: \
            tickFn(o); \
        case 2: \
            return 0; \
        } \
        return 0; \
    }

#define ROOMLIB_RESET_SIGNAL_WITH_TARGET_GATE(name) \
    int name(RoomEnt *o) { \
        struct RoomSub *s = &o->sub; \
        o->state = 4; \
        o->flag3 = 0; \
        if (o->bB8 == 0) { \
            if (o->active != 0) { \
                if (o->link->target != 0) { \
                    *o->link->target->state = 4; \
                } \
            } \
        } else { \
            if (o->link->target != 0) { \
                unsigned char *state = o->link->target->state; \
                if (*state == 1) { \
                    *state = 4; \
                } \
            } \
        } \
        if (s->signal != 0) { \
            *s->signal = 0; \
        } \
        return 0; \
    }

#define ROOMLIB_ARM_IF_WINDOW_VIA(name, handler) \
    void name(RoomEnt *o) { \
        RoomLink *l = o->link->link18C; \
        if (o->t16 >= 0) { \
            if (o->t16 != l->variant) return; \
        } \
        if (o->t17 >= 0) { \
            int hi = l->winHi; \
            int lo = l->winLo; \
            if (o->t17 < hi || lo < o->t17) return; \
        } \
        o->sub.cb = handler; \
    }

typedef struct RoomTimer {
    char pad0[0x24];
    unsigned short h24;           /* 0x24: reload value */
    short h26;                    /* 0x26: countdown */
    short h28;                    /* 0x28: fire request */
} RoomTimer;

typedef struct RoomTimer2 {
    char pad0[0x28];
    unsigned short h28;           /* 0x28: reload value */
    short h2A;                    /* 0x2A: countdown */
    short h2C;                    /* 0x2C: fire request */
} RoomTimer2;

extern int func_800C6C18();
extern int func_800C2B68();

#define ROOMLIB_TIMER_TICK(name) \
    void name(int a, unsigned char *st, RoomTimer *t) { \
        short n = t->h26; \
        RoomTimer *p = t; \
        if (n != 0) { \
            p->h26 = n - 1; \
        } \
        if (p->h28 == 1) { \
            p->h28 = 0; \
            if (p->h26 == 0) { \
                p->h26 = p->h24; \
                func_800C6C18(); \
            } \
        } \
        if (func_800C2B68() == 1) { \
            st[1] = 2; \
        } \
    }

#define ROOMLIB_TIMER_TICK2(name) \
    void name(int a, unsigned char *st, RoomTimer2 *t) { \
        register unsigned char *state asm("s0") = st; \
        short n = t->h2A; \
        register RoomTimer2 *p asm("a1") = t; \
        if (n != 0) { \
            t->h2A = n - 1; \
        } \
        if (p->h2C == 1) { \
            p->h2C = 0; \
            if (p->h2A == 0) { \
                p->h2A = p->h28; \
                func_800C6C18(); \
            } \
        } \
        if (func_800C2B68() == 1) { \
            state[1] = 2; \
        } \
    }

typedef struct RoomFxParams {
    char pad0[0x8];
    short h8;
    short hA;
    short hC;
    char padE[0x2];
    short h10;
    short h12;
    char pad14[0x1];
    unsigned char b15;
    unsigned char b16;
} RoomFxParams;

#define ROOMLIB_SET3_SIZE(name) \
    void name(int a, int b, RoomFxParams *c) { \
        c->h8 = 0xC8; \
        c->hA = 0x80; \
    }

#define ROOMLIB_SET3_RANGE(name) \
    void name(int a, int b, RoomFxParams *c) { \
        c->h10 = 0x258; \
        c->h12 = 0x80; \
        c->b15 = 0; \
    }

#define ROOMLIB_SET3_RESET(name, v10, v12) \
    void name(int a, int b, RoomFxParams *c) { \
        c->h10 = v10; \
        c->h8 = 0; \
        c->hA = 0; \
        c->hC = 0; \
        c->h12 = v12; \
    }

typedef struct RoomStatePair {
    unsigned char b0;
    unsigned char b1;             /* 0x01: state byte, 2 = done */
    short h2;                     /* 0x02: threshold */
} RoomStatePair;

typedef struct RoomClock {
    char pad0[0xC];
    short hC;                     /* 0x0C: current tick */
} RoomClock;

extern RoomClock *func_800C2B50();

#define ROOMLIB_FX_SHIMMER(name) \
    void name(int a, RoomStatePair *st, RoomFxParams *c) { \
        RoomClock *r = func_800C2B50(); \
        c->h10 += 3; \
        if (r->hC - 0x1E < st->h2) { \
            unsigned char v = c->b16 + 8; \
            c->b16 = v; \
            c->b15 = v >> 4; \
            if (v >> 4 == 8) { \
                st->b1 = 2; \
            } \
        } \
    }

typedef struct RoomPartVec {
    unsigned short x;
    unsigned short y;
    unsigned short z;
    short pad6;
} RoomPartVec;

typedef struct RoomPartSys {
    char pad0[0x20];
    RoomPartVec pos[6];           /* 0x20 */
    RoomPartVec vel[6];           /* 0x50 */
    char pad80[0x18];
    short flag[6];                /* 0x98: 1 = live */
    short hA4;                    /* 0xA4: live count */
} RoomPartSys;

/* per-frame particle step: velocity/16 integration, +4 gravity, kill on
 * floor hit; when the last one dies flag the state pair */
#define ROOMLIB_PARTICLE_STEP(name) \
    void name(int a, unsigned char *st, RoomPartSys *sys) { \
        int i; \
        int grav; \
        unsigned short *gp; \
        for (i = 0; (unsigned)i < 6; i++) { \
            grav = 4; \
            if (sys->flag[i] == 1) { \
                int vx = (short)sys->vel[i].x >> grav; \
                int vy = (short)sys->vel[i].y >> grav; \
                int vz = (short)sys->vel[i].z >> grav; \
                sys->pos[i].x += vx; \
                sys->pos[i].y = sys->pos[i].y + vy; \
                sys->pos[i].z += vz; \
                gp = &g_FrameCount16; \
                sys->vel[i].y += grav; \
                if ((short)sys->pos[i].y >= (short)*gp) { \
                    short r; \
                    sys->flag[i] = 0; \
                    sys->hA4 = (r = sys->hA4 - 1); \
                    if (r == 0) { \
                        st[1] = 2; \
                    } \
                } \
            } \
        } \
    }

#define ROOMLIB_REGISTER_TABLE_AT3(name, table) \
    int name(void *o) { \
        if (FieldEng_GetStatus() == 3) { \
            FieldEng_Register(o, table); \
        } \
        return 0; \
    }

/* INIT_E: timers/handler like INIT_D plus render-node wake bits */
#define ROOMLIB_INIT_E(name, handler) \
    int name(RoomEnt *o) { \
        unsigned int m = 0x10002; \
        RoomLink *l = o->link; \
        o->t16 = -1; \
        o->t17 = -1; \
        o->t18 = -1; \
        o->t19 = 3; \
        o->sub.signal = 0; \
        o->active = 0; \
        o->t1A = 0; \
        RW16(o, 0x32) = 0; \
        RW16(o, 0x36) = 0; \
        o->sub.cb = handler; \
        RW32(l, 0x98) |= m; \
        RWU16(l, 0x250) |= 0x400; \
        return 0; \
    }

#endif
