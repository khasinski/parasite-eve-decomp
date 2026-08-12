/* Shared room-script library. Every room links its own copy; instance
 * functions are stamped per room/offset with these body macros. */
#ifndef ROOM_LIB_H
#define ROOM_LIB_H

#include "../../../include/common.h"
#include "../../../include/pe1/gte.h"
#include "../../../include/pe1/room_fx.h"

typedef struct RoomObj {
    char pad0[0xC];
    void (*callback)(void);
} RoomObj;

/* 20-byte effect record allocated by the room particle handlers. */
typedef struct RoomParticleState {
    short x;                      /* 0x00 */
    short y;                      /* 0x02 */
    short z;                      /* 0x04 */
    short size;                   /* 0x06 */
    short vx;                     /* 0x08 */
    short vy;                     /* 0x0A */
    short vz;                     /* 0x0C */
    short angle;                  /* 0x0E */
    short state;                  /* 0x10 */
    short timer;                  /* 0x12 */
} RoomParticleState;

/* Parameters read by the matching room particle emitter handlers. */
typedef struct RoomParticleEmitter {
    short x;                      /* 0x00 */
    short y;                      /* 0x02 */
    short z;                      /* 0x04 */
    short soundId;                /* 0x06: -1 disables the completion sound */
    int radialMagnitude;          /* 0x08 */
    int spawnPeriod;              /* 0x0C */
    int endFrame;                 /* 0x10 */
} RoomParticleEmitter;

#define ROOMLIB_INVOKE_CALLBACK_C(name) \
    int name(RoomObj *obj) { \
        obj->callback(); \
        return 0; \
    }

#define ROOMLIB_RETURN_ZERO(name) \
    int name(void) { \
        return 0; \
    }

#define ROOMLIB_RETURN_VOID(name) \
    void name(void) { \
    }

#define RW32(o, off) (*(int *)((char *)(o) + (off)))
#define RW16(o, off) (*(short *)((char *)(o) + (off)))
#define RW8(o, off)  (*(unsigned char *)((char *)(o) + (off)))
#define RWU16(o, off) (*(unsigned short *)((char *)(o) + (off)))
#define RVW32(o, off) (*(volatile int *)((char *)(o) + (off)))
#define RVW16(o, off) (*(volatile short *)((char *)(o) + (off)))
#define RVU16(o, off) (*(volatile unsigned short *)((char *)(o) + (off)))
#define RWPTR(o, off) ((void *)((char *)(o) + (off)))

/* Preserve a signed halfword load when the matching compiler would otherwise
 * replace it with lhu because the value is immediately narrowed again. */
#define ROOMLIB_LOAD_S16(out, address) \
    asm volatile("lh %0,%1" : "=r"(out) : "m"(*(short *)(address)))

/* Register allocation used by the original compiler for room rotation-table
 * lookups. Keeping it named here avoids scattering compiler pins in logic. */
#define ROOMLIB_ROT_ENTRY_DECL \
    register int *entry asm("$2"); \
    register int *base asm("$3")

#define ROOMLIB_V0_PTR_DECL(name) register void *name asm("$2")
#define ROOMLIB_V1_INT_DECL(name) register int name asm("$3")
#define ROOMLIB_A0_INT_DECL(name) register int name asm("$4")
#define ROOMLIB_LOAD_PTR(out, address) \
    asm volatile("lw %0,%1" : "=r"(out) : "m"(*(void **)(address)))
#define ROOMLIB_LOAD_U16(out, address) \
    asm volatile("lhu %0,%1" : "=r"(out) : "m"(*(unsigned short *)(address)))

#define ROOMLIB_DIV_V0_A0_CHECKED(value, denominator) \
    asm volatile( \
        ".word 0x0044001A\n\t" \
        ".word 0x14800002\n\t" \
        ".word 0x00000000\n\t" \
        ".word 0x0007000D\n\t" \
        ".word 0x2401FFFF\n\t" \
        ".word 0x14810004\n\t" \
        ".word 0x3C018000\n\t" \
        ".word 0x14410002\n\t" \
        ".word 0x00000000\n\t" \
        ".word 0x0006000D\n\t" \
        ".word 0x00001012\n\t" \
        ".word 0x00000000" \
        : "=r"(value) \
        : "0"(value), "r"(denominator) \
        : "$1", "lo")

typedef struct RoomLibTick12Rec {
    char pad00[0x20];
    unsigned short frameStep;     /* 0x20 */
    short timerLimit;             /* 0x22 */
    char pad24[0x2];
    unsigned short repeatCount;   /* 0x26 */
    short phaseLimit;             /* 0x28 */
    unsigned short phaseStep;     /* 0x2A */
    signed char state[12];        /* 0x2C */
    signed char timer[12];        /* 0x38 */
    unsigned short phase[12];     /* 0x44 */
    char pad5C[0x4];
    unsigned short frame[12][4];  /* 0x60 */
} RoomLibTick12Rec;

#define ROOMLIB_TICK_12_COUNTERS(name) \
    void name(void *arg0, unsigned char *signal, RoomLibTick12Rec *rec) { \
        unsigned int i; \
        for (i = 0; i < 12; i++) { \
            int s = rec->state[i]; \
            if (s != -1) { \
                if (s == 1) { \
                    short limit; \
                    rec->phase[i] += rec->phaseStep; \
                    limit = rec->phaseLimit; \
                    if (limit < (short)rec->phase[i]) { \
                        rec->phase[i] = limit; \
                    } \
                } \
                rec->frame[i][0] -= rec->frameStep; \
                if ((short)rec->frame[i][0] < 10) { \
                    rec->state[i] = 1; \
                } \
            } \
            rec->timer[i]++; \
            if (rec->timer[i] == rec->timerLimit) { \
                rec->repeatCount--; \
                if ((short)rec->repeatCount == 0) { \
                    signal[1] = 2; \
                } \
            } \
        } \
    }





extern void RoomLib_HandlerA();
extern void RoomLib_HandlerB(void);
extern void RoomLib_HandlerC(void);
extern void RoomLib_HandlerD(void);
extern void RoomLib_HandlerE();
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
extern int func_8003010C(void *o, int arg);
extern void func_80030220(void *o, int arg, int value);
extern void func_80192BFC(void);

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
    int pos[4];                   /* 0x28: 16.16 x/y/z plus copied trailing word */
    char pad38[0x2];
    unsigned short h3A;           /* 0x3A */
    char pad3C[0x4];
    int posMirror[4];             /* 0x40: optional position snapshot */
    char pad50[0x18];
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

/* Working records used while rotating scripted room motion vectors. */
typedef struct RoomLibMotionState {
    char pad00[0x40];
    int position[3];              /* 0x40 */
    union {
        int word;
        struct {
            short pad4C;
            short height;
        } half;
    } mode;                       /* 0x4C */
    int anchor[3];                /* 0x50 */
    char pad5C[0x14];
    int localStep[3];             /* 0x70 */
    char pad7C[0x4];
    RoomLink *target;             /* 0x80 */
} RoomLibMotionState;

typedef struct RoomLibMotionWork {
    short input[3];               /* 0x00 */
    short pad06;
    int rotated[3];               /* 0x08 */
    char pad14[0x14];
    short matrix[9];              /* 0x28 */
    short pad3A;
} RoomLibMotionWork;

/* Motion state addressed by HandlerE through RoomEnt + 0x0C. */
typedef struct RoomLibHandlerEState {
    void (*callback)(void);       /* 0x00 */
    int *signal;                  /* 0x04 */
    short active;                 /* 0x08 */
    signed char variant;          /* 0x0A */
    unsigned char optionB;        /* 0x0B */
    unsigned char optionC;        /* 0x0C */
    unsigned char flags;          /* 0x0D */
    char pad0E[0x2];
    int start[3];                 /* 0x10 */
    char pad1C[0x4];
    int target[3];                /* 0x20 */
    char pad2C[0x4];
    int delta[3];                 /* 0x30 */
    char pad3C[0x4];
    int localOffset[3];           /* 0x40 */
    char pad4C[0x4];
    int secondary[3];             /* 0x50 */
    char pad5C[0x4];
    RoomLink *targetLink;         /* 0x60 */
    RoomLink *secondaryLink;      /* 0x64 */
    int speed;                    /* 0x68 */
    short duration;               /* 0x6C */
    short secondaryHeading;       /* 0x6E */
    short eased;                  /* 0x70 */
    short heading;                /* 0x72 */
    short mirrorPosition;         /* 0x74 */
    short frameLimit;             /* 0x76 */
    short frame;                  /* 0x78 */
    short phase;                  /* 0x7A */
    short phaseFrame[4];          /* 0x7C */
    unsigned char lockY;          /* 0x84 */
    unsigned char copyPosition;  /* 0x85 */
} RoomLibHandlerEState;

/* Argument-controlled state used by HandlerD through RoomEnt + 0x0C. */
typedef struct RoomLibHandlerDState {
    void (*callback)(void);       /* 0x00 */
    int *signal;                  /* 0x04 */
    short active;                 /* 0x08 */
    signed char variant;          /* 0x0A */
    unsigned char optionB;        /* 0x0B */
    unsigned char optionC;        /* 0x0C */
    unsigned char flags;          /* 0x0D */
    char pad0E[0x32];
    int target[4];                /* 0x40 */
    char pad50[0x10];
    int positionX;                /* 0x60 */
    char pad64[0x4];
    int positionZ;                /* 0x68 */
    char pad6C[0x4];
    short rotation[3];            /* 0x70 */
    char pad76[0xA];
    RoomLink *targetLink;         /* 0x80 */
    RoomLink *secondaryLink;      /* 0x84 */
    int value88;                  /* 0x88 */
    int value8C;                  /* 0x8C */
    int value90;                  /* 0x90 */
    char pad94[0x6];
    short stateValue;             /* 0x9A */
    short range[3];               /* 0x9C */
    short heading;                /* 0xA2 */
    short duration;               /* 0xA4 */
    short rate;                   /* 0xA6 */
    unsigned char mirrorPosition; /* 0xA8 */
    unsigned char reverse;        /* 0xA9 */
    unsigned char lockY;          /* 0xAA */
    unsigned char copyPosition;   /* 0xAB */
    unsigned char copyRotation;   /* 0xAC */
} RoomLibHandlerDState;

/* Argument-controlled state used by HandlerB through RoomEnt + 0x0C. */
typedef struct RoomLibHandlerBState {
    void (*callback)(void);       /* 0x00 */
    int *signal;                  /* 0x04 */
    short active;                 /* 0x08 */
    signed char variant;          /* 0x0A */
    unsigned char optionB;        /* 0x0B */
    unsigned char optionC;        /* 0x0C */
    unsigned char flags;          /* 0x0D */
    char pad0E[0x22];
    int target[3];                /* 0x30 */
    char pad3C[0x4];
    int localOffset[3];           /* 0x40 */
    char pad4C[0x4];
    int secondaryX;               /* 0x50 */
    char pad54[0x4];
    int secondaryZ;               /* 0x58 */
    char pad5C[0x4];
    RoomLink *targetLink;         /* 0x60 */
    RoomLink *secondaryLink;      /* 0x64 */
    int speed;                    /* 0x68 */
    int acceleration;             /* 0x6C */
    int duration;                 /* 0x70 */
    int phaseValue;               /* 0x74 */
    char pad78[0x8];
    int mode;                     /* 0x80 */
    char pad84[0x2];
    short rate;                   /* 0x86 */
    short heading;                /* 0x88 */
    short secondaryHeading;       /* 0x8A */
} RoomLibHandlerBState;

/* Argument-controlled state used by HandlerC through RoomEnt + 0x0C. */
typedef struct RoomLibHandlerCState {
    void (*callback)(void);       /* 0x00 */
    int *signal;                  /* 0x04 */
    short active;                 /* 0x08 */
    signed char variant;          /* 0x0A */
    unsigned char optionB;        /* 0x0B */
    unsigned char optionC;        /* 0x0C */
    unsigned char flags;          /* 0x0D */
    char pad0E[0x22];
    int target[3];                /* 0x30 */
    char pad3C[0x14];
    int localOffset[3];           /* 0x50 */
    char pad5C[0x4];
    int secondaryX;               /* 0x60 */
    char pad64[0x4];
    int secondaryZ;               /* 0x68 */
    char pad6C[0x4];
    RoomLink *targetLink;         /* 0x70 */
    RoomLink *secondaryLink;      /* 0x74 */
    char pad78[0x4];
    int phaseValue;               /* 0x7C */
    char pad80[0x8];
    int mode;                     /* 0x88 */
    char pad8C[0x4];
    short rate;                   /* 0x90 */
    short heading;                /* 0x92 */
    short secondaryHeading;       /* 0x94 */
} RoomLibHandlerCState;

#define ROOMLIB_HANDLER_C_ARGS(name, armHandler, phaseHandler) \
    int name(RoomEnt *o, int query, unsigned int op, int arg0, int arg1, int arg2) { \
        RoomLibHandlerCState *state = (RoomLibHandlerCState *)&o->sub; \
        FieldActorNode *node; \
        switch (op) { \
        case 19: \
            if (query == 0) { \
                o->flag3 = arg0; \
            } else { \
                *(int *)arg0 = o->flag3; \
            } \
            break; \
        case 25: \
            if (query == 1) { \
                state->signal = (int *)arg0; \
                *(int *)arg0 = query; \
            } \
            break; \
        case 0: \
            state->targetLink = (RoomLink *)D_8009D20C; \
            while (state->targetLink != 0) { \
                node = (FieldActorNode *)state->targetLink; \
                if (node->b0C == arg0 && node->b0D == arg1 && \
                    (node->w98 & 0x10) == 0) { \
                    break; \
                } \
                state->targetLink = (RoomLink *) \
                    ((FieldActorNode *)state->targetLink)->next; \
            } \
            break; \
        case 17: \
            state->target[0] = arg0; \
            state->target[2] = arg2; \
            if (arg1 == -1) { \
                state->target[1] = RW32(D_8009D254, 0x2C); \
            } else { \
                state->target[1] = arg1; \
            } \
            state->targetLink = 0; \
            break; \
        case 15: \
            state->rate = arg0; \
            break; \
        case 13: \
            state->mode = arg0; \
            break; \
        case 6: \
            state->localOffset[0] = arg0; \
            state->localOffset[1] = arg1; \
            state->localOffset[2] = arg2; \
            break; \
        case 23: \
            arg0 = arg0 != 0; \
            arg1 = (arg1 != 0) << 1; \
            arg2 = (arg2 != 0) << 2; \
            state->flags = arg0 | arg1 | arg2; \
            break; \
        case 10: \
            state->variant = arg0; \
            state->active = arg1; \
            if (o->flag3 == 1) { \
                state->callback = (void (*)(void))armHandler; \
            } \
            break; \
        case 11: \
            if (o->flag3 == 1) { \
                state->optionB = arg0; \
                state->optionC = arg1; \
            } \
            break; \
        case 16: \
            state->heading = arg0; \
            break; \
        case 18: \
            if (o->flag3 == 2) { \
                state->callback = (void (*)(void))phaseHandler; \
                state->phaseValue = arg0; \
                o->flag3 = 3; \
            } \
            break; \
        case 21: \
            state->secondaryLink = (RoomLink *)D_8009D20C; \
            while (state->secondaryLink != 0) { \
                node = (FieldActorNode *)state->secondaryLink; \
                if (node->b0C == arg0 && node->b0D == arg1 && \
                    (node->w98 & 0x10) == 0) { \
                    break; \
                } \
                state->secondaryLink = (RoomLink *) \
                    ((FieldActorNode *)state->secondaryLink)->next; \
            } \
            state->secondaryHeading = arg2; \
            break; \
        case 22: \
            state->secondaryX = arg0; \
            state->secondaryZ = arg1; \
            state->secondaryHeading = arg2; \
            break; \
        } \
        return 0; \
    }

#define ROOMLIB_HANDLER_B_ARGS(name, armHandler, phaseHandler) \
    int name(RoomEnt *o, int query, unsigned int op, int arg0, int arg1, int arg2) { \
        RoomLibHandlerBState *state = (RoomLibHandlerBState *)&o->sub; \
        FieldActorNode *node; \
        switch (op) { \
        case 19: \
            if (query == 0) { \
                o->flag3 = arg0; \
            } else { \
                *(int *)arg0 = o->flag3; \
            } \
            break; \
        case 25: \
            if (query == 1) { \
                state->signal = (int *)arg0; \
                *(int *)arg0 = query; \
            } \
            break; \
        case 4: \
            state->speed = arg0; \
            state->acceleration = arg1; \
            break; \
        case 2: \
            state->duration = arg0; \
            break; \
        case 12: \
            state->phaseValue = arg0; \
            break; \
        case 13: \
            state->mode = arg0; \
            break; \
        case 14: \
            state->rate = arg1; \
            break; \
        case 6: \
            state->localOffset[0] = arg0; \
            state->localOffset[1] = arg1; \
            state->localOffset[2] = arg2; \
            break; \
        case 23: \
            arg0 = arg0 != 0; \
            arg1 = (arg1 != 0) << 1; \
            arg2 = (arg2 != 0) << 2; \
            state->flags = arg0 | arg1 | arg2; \
            break; \
        case 10: \
            state->variant = arg0; \
            state->active = arg1; \
            state->callback = (void (*)(void))armHandler; \
            break; \
        case 11: \
            state->optionB = arg0; \
            state->optionC = arg1; \
            break; \
        case 16: \
            state->heading = arg0; \
            break; \
        case 18: \
            if (o->flag3 == 2) { \
                state->callback = (void (*)(void))phaseHandler; \
                state->phaseValue = arg0; \
                o->flag3 = 3; \
            } \
            break; \
        case 0: \
            state->targetLink = (RoomLink *)D_8009D20C; \
            while (state->targetLink != 0) { \
                node = (FieldActorNode *)state->targetLink; \
                if (node->b0C == arg0 && node->b0D == arg1 && \
                    (node->w98 & 0x10) == 0) { \
                    break; \
                } \
                state->targetLink = (RoomLink *) \
                    ((FieldActorNode *)state->targetLink)->next; \
            } \
            break; \
        case 17: \
            state->target[0] = arg0; \
            state->target[2] = arg2; \
            if (arg1 != -1) { \
                state->target[1] = arg1; \
            } else { \
                state->target[1] = RW32(D_8009D254, 0x2C); \
            } \
            state->targetLink = 0; \
            break; \
        case 21: \
            state->secondaryLink = (RoomLink *)D_8009D20C; \
            while (state->secondaryLink != 0) { \
                node = (FieldActorNode *)state->secondaryLink; \
                if (node->b0C == arg0 && node->b0D == arg1 && \
                    (node->w98 & 0x10) == 0) { \
                    break; \
                } \
                state->secondaryLink = (RoomLink *) \
                    ((FieldActorNode *)state->secondaryLink)->next; \
            } \
            state->secondaryHeading = arg2; \
            break; \
        case 22: \
            state->secondaryX = arg0; \
            state->secondaryZ = arg1; \
            state->secondaryHeading = arg2; \
            break; \
        } \
        return 0; \
    }

#define ROOMLIB_HANDLER_D_ARGS(name, notifyHandler) \
    int name(RoomEnt *o, int query, unsigned int op, int arg0, int arg1, int arg2) { \
        RoomLibHandlerDState *state = (RoomLibHandlerDState *)&o->sub; \
        FieldActorNode *node; \
        switch (op) { \
        case 19: \
            if (query == 0) { \
                o->flag3 = arg0; \
            } else { \
                *(int *)arg0 = o->flag3; \
                *(int *)arg1 = state->stateValue; \
            } \
            break; \
        case 25: \
            if (query == 1) { \
                state->signal = (int *)arg0; \
                *(int *)arg0 = query; \
            } \
            break; \
        case 0: \
            state->targetLink = (RoomLink *)D_8009D20C; \
            while (state->targetLink != 0) { \
                node = (FieldActorNode *)state->targetLink; \
                if (node->b0C == arg0 && node->b0D == arg1 && \
                    (node->w98 & 0x10) == 0) { \
                    break; \
                } \
                state->targetLink = (RoomLink *) \
                    ((FieldActorNode *)state->targetLink)->next; \
            } \
            break; \
        case 17: \
            state->target[0] = arg0; \
            state->target[2] = arg2; \
            if (arg1 == -1) { \
                state->target[1] = RW32(D_8009D254, 0x2C); \
            } else { \
                state->target[1] = arg1; \
            } \
            state->targetLink = 0; \
            break; \
        case 26: \
            state->target[0] = arg0; \
            state->target[1] = o->link->pos[1]; \
            state->target[2] = arg1; \
            state->target[3] = arg2; \
            state->targetLink = 0; \
            break; \
        case 4: \
            state->value88 = arg0; \
            break; \
        case 2: \
            state->value8C = arg0; \
            break; \
        case 3: \
            state->value90 = arg0; \
            state->rate = arg1; \
            if (arg2 > 0) { \
                state->duration = arg2; \
            } \
            break; \
        case 23: \
            arg0 = arg0 != 0; \
            arg1 = (arg1 != 0) << 1; \
            arg2 = (arg2 != 0) << 2; \
            state->flags = arg0 | arg1 | arg2; \
            break; \
        case 10: \
            state->variant = arg0; \
            state->active = arg1; \
            state->mirrorPosition = arg2; \
            state->callback = (void (*)(void))notifyHandler; \
            break; \
        case 11: \
            state->optionB = arg0; \
            state->optionC = arg1; \
            break; \
        case 6: \
            state->rotation[0] = arg0 >> 16; \
            state->rotation[1] = arg1 >> 16; \
            state->rotation[2] = arg2 >> 16; \
            break; \
        case 20: \
            state->range[0] = arg0; \
            state->range[1] = arg1; \
            state->range[2] = arg2; \
            break; \
        case 21: \
            state->secondaryLink = (RoomLink *)D_8009D20C; \
            while (state->secondaryLink != 0) { \
                node = (FieldActorNode *)state->secondaryLink; \
                if (node->b0C == arg0 && node->b0D == arg1 && \
                    (node->w98 & 0x10) == 0) { \
                    break; \
                } \
                state->secondaryLink = (RoomLink *) \
                    ((FieldActorNode *)state->secondaryLink)->next; \
            } \
            state->heading = arg2 & 0xFFF; \
            state->reverse = (unsigned int)arg2 >> 31; \
            break; \
        case 22: \
            state->positionX = arg0; \
            state->positionZ = arg1; \
            state->heading = arg2 & 0xFFF; \
            state->reverse = (unsigned int)arg2 >> 31; \
            break; \
        case 30: \
            state->lockY = arg0; \
            break; \
        case 31: \
            state->copyPosition = arg0; \
            state->copyRotation = arg1; \
            break; \
        } \
        return 0; \
    }

#define ROOMLIB_HANDLER_E_ARGS(name, notifyHandler) \
    int name(RoomEnt *o, int query, unsigned int op, int arg0, int arg1, int arg2) { \
        RoomLibHandlerEState *state = (RoomLibHandlerEState *)&o->sub; \
        FieldActorNode *node; \
        switch (op) { \
        case 19: \
            if (query == 0) { \
                o->flag3 = arg0; \
            } else { \
                *(int *)arg0 = o->flag3; \
            } \
            break; \
        case 25: \
            if (query == 1) { \
                state->signal = (int *)arg0; \
                *(int *)arg0 = query; \
            } \
            break; \
        case 0: \
            state->targetLink = (RoomLink *)D_8009D20C; \
            while (state->targetLink != 0) { \
                node = (FieldActorNode *)state->targetLink; \
                if (node->b0C == arg0 && node->b0D == arg1 && \
                    (node->w98 & 0x10) == 0) { \
                    break; \
                } \
                state->targetLink = (RoomLink *) \
                    ((FieldActorNode *)state->targetLink)->next; \
            } \
            break; \
        case 17: \
            state->target[0] = arg0; \
            state->target[2] = arg2; \
            if (arg1 == -1) { \
                state->target[1] = RW32(D_8009D254, 0x2C); \
            } else { \
                state->target[1] = arg1; \
            } \
            state->targetLink = 0; \
            break; \
        case 4: \
            state->speed = arg0; \
            state->eased = arg1; \
            state->duration = 0; \
            break; \
        case 6: \
            state->localOffset[0] = arg0; \
            state->localOffset[1] = arg1; \
            state->localOffset[2] = arg2; \
            break; \
        case 15: \
            state->duration = arg0; \
            state->eased = arg1; \
            break; \
        case 21: \
            state->secondaryLink = (RoomLink *)D_8009D20C; \
            while (state->secondaryLink != 0) { \
                node = (FieldActorNode *)state->secondaryLink; \
                if (node->b0C == arg0 && node->b0D == arg1 && \
                    (node->w98 & 0x10) == 0) { \
                    break; \
                } \
                state->secondaryLink = (RoomLink *) \
                    ((FieldActorNode *)state->secondaryLink)->next; \
            } \
            state->secondaryHeading = arg2; \
            break; \
        case 22: \
            state->secondary[0] = arg0; \
            state->secondary[2] = arg1; \
            state->secondaryHeading = arg2; \
            break; \
        case 23: \
            arg0 = arg0 != 0; \
            arg1 = (arg1 != 0) << 1; \
            arg2 = (arg2 != 0) << 2; \
            state->flags = arg0 | arg1 | arg2; \
            break; \
        case 10: \
            state->variant = arg0; \
            state->active = arg1; \
            state->mirrorPosition = arg2; \
            state->callback = (void (*)(void))notifyHandler; \
            break; \
        case 11: \
            state->optionB = arg0; \
            state->optionC = arg1; \
            break; \
        case 24: \
            if (query == 1) { \
                *(int *)arg0 = state->heading; \
            } \
            break; \
        case 27: \
            state->phaseFrame[1] = arg0; \
            state->phaseFrame[2] = arg1; \
            state->phaseFrame[3] = arg2; \
            if (arg0 != -1) { \
                if (arg1 != -1) { \
                    if (arg2 == -1) { \
                        state->frameLimit = 2; \
                    } else { \
                        state->frameLimit = 3; \
                    } \
                } else { \
                    state->frameLimit = 1; \
                } \
            } else { \
                state->frameLimit = 0; \
            } \
            break; \
        case 31: \
            state->lockY = arg0; \
            break; \
        case 32: \
            state->copyPosition = arg0; \
            break; \
        } \
        return 0; \
    }
extern short D_800966EE[];
extern short D_800966EC[][2];
extern char *D_8009D254;
struct FieldActorNode;
extern struct FieldActorNode *D_8009D20C;
extern void RoomLib_AdvanceArcToTarget_80191110(RoomEnt *obj);
extern void RoomLib_AdvanceArcToTargetY_80191D18(RoomEnt *obj);
extern int RoomLib_ResetAndSignal_801914B0(RoomEnt *obj);
extern void RoomLib_ArmWindowA_80190D0C(RoomEnt *obj);
extern void RoomLib_ArmWindowB_80191824(RoomEnt *obj);
extern void RoomLib_NotifyArmB_8018F598(RoomEnt *obj);
extern void RoomLib_Notify2ArmB_801902D8(RoomEnt *obj);
extern void RoomLib_Notify2ArmB_801902C8(RoomEnt *obj);
extern void RoomLib_Notify2ArmB_801902D0(RoomEnt *obj);
extern void RoomLib_Notify2ArmB_801902D4(RoomEnt *obj);
extern void RoomLib_Notify2ArmB_801902DC(RoomEnt *obj);
extern void RoomLib_Notify2ArmB_801902E0(RoomEnt *obj);
extern void RoomLib_Notify2ArmB_801902E4(RoomEnt *obj);
extern void RoomLib_Notify2ArmB_801902E8(RoomEnt *obj);
extern void RoomLib_Notify2ArmB_801902EC(RoomEnt *obj);
extern void RoomLib_Notify2ArmB_801902FC(RoomEnt *obj);
extern void RoomLib_Notify2ArmB_8019031C(RoomEnt *obj);
extern void RoomLib_Notify2ArmB_80190314(RoomEnt *obj);
extern void RoomLib_Notify2ArmB_80190320(RoomEnt *obj);
extern void RoomLib_Notify2ArmB_80190328(RoomEnt *obj);
extern void RoomLib_Notify2ArmB_80190340(RoomEnt *obj);
extern void RoomLib_Notify2ArmB_8019035C(RoomEnt *obj);
extern void RoomLib_Notify2ArmB_80190F64(RoomEnt *obj);
extern void RoomLib_Notify2ArmB_801912CC(RoomEnt *obj);
extern void RoomLib_Notify2ArmB_801912D4(RoomEnt *obj);
extern void RoomLib_Notify2ArmB_801912E0(RoomEnt *obj);
extern void RoomLib_Notify2ArmB_80191368(RoomEnt *obj);
extern void RoomLib_Notify2ArmB_80191430(RoomEnt *obj);
extern void RoomLib_Notify2ArmB_801917E4(RoomEnt *obj);
extern void RoomLib_Notify2ArmB_80191D00(RoomEnt *obj);
extern void RoomLib_Notify2ArmB_80191F18(RoomEnt *obj);
extern void RoomLib_Notify2ArmB_80192420(RoomEnt *obj);
extern void RoomLib_Notify2ArmB_80192428(RoomEnt *obj);
extern void RoomLib_Notify2ArmB_80194A5C(RoomEnt *obj);
extern void RoomLib_NotifyArmB_8018F588(RoomEnt *obj);
extern void RoomLib_NotifyArmB_8018F590(RoomEnt *obj);
extern void RoomLib_NotifyArmB_8018F594(RoomEnt *obj);
extern void RoomLib_NotifyArmB_8018F59C(RoomEnt *obj);
extern void RoomLib_NotifyArmB_8018F5A0(RoomEnt *obj);
extern void RoomLib_NotifyArmB_8018F5A4(RoomEnt *obj);
extern void RoomLib_NotifyArmB_8018F5A8(RoomEnt *obj);
extern void RoomLib_NotifyArmB_8018F5AC(RoomEnt *obj);
extern void RoomLib_NotifyArmB_8018F5BC(RoomEnt *obj);
extern void RoomLib_NotifyArmB_8018F5DC(RoomEnt *obj);
extern void RoomLib_NotifyArmB_8018F5E0(RoomEnt *obj);
extern void RoomLib_NotifyArmB_8018F5E8(RoomEnt *obj);
extern void RoomLib_NotifyArmB_8018F600(RoomEnt *obj);
extern void RoomLib_NotifyArmB_8018F61C(RoomEnt *obj);
extern void RoomLib_NotifyArmB_80190224(RoomEnt *obj);
extern void RoomLib_NotifyArmB_8019058C(RoomEnt *obj);
extern void RoomLib_NotifyArmB_80190594(RoomEnt *obj);
extern void RoomLib_NotifyArmB_801905A0(RoomEnt *obj);
extern void RoomLib_NotifyArmB_80190628(RoomEnt *obj);
extern void RoomLib_NotifyArmB_801906F0(RoomEnt *obj);
extern void RoomLib_NotifyArmB_80190AA4(RoomEnt *obj);
extern void RoomLib_NotifyArmB_80190FC0(RoomEnt *obj);
extern void RoomLib_NotifyArmB_801911D8(RoomEnt *obj);
extern void RoomLib_NotifyArmB_801916E0(RoomEnt *obj);
extern void RoomLib_NotifyArmB_801916E8(RoomEnt *obj);
extern void RoomLib_NotifyArmB_80193D1C(RoomEnt *obj);
extern void RoomLib_ArmWindowA_80190CFC(RoomEnt *obj);
extern void RoomLib_ArmWindowA_80190D04(RoomEnt *obj);
extern void RoomLib_ArmWindowA_80190D08(RoomEnt *obj);
extern void RoomLib_ArmWindowA_80190D10(RoomEnt *obj);
extern void RoomLib_ArmWindowA_80190D14(RoomEnt *obj);
extern void RoomLib_ArmWindowA_80190D18(RoomEnt *obj);
extern void RoomLib_ArmWindowA_80190D1C(RoomEnt *obj);
extern void RoomLib_ArmWindowA_80190D20(RoomEnt *obj);
extern void RoomLib_ArmWindowA_80190D30(RoomEnt *obj);
extern void RoomLib_ArmWindowA_80190D50(RoomEnt *obj);
extern void RoomLib_ArmWindowA_80190D54(RoomEnt *obj);
extern void RoomLib_ArmWindowA_80190D5C(RoomEnt *obj);
extern void RoomLib_ArmWindowA_80190D74(RoomEnt *obj);
extern void RoomLib_ArmWindowA_80190D90(RoomEnt *obj);
extern void RoomLib_ArmWindowA_80191998(RoomEnt *obj);
extern void RoomLib_ArmWindowA_80191D00(RoomEnt *obj);
extern void RoomLib_ArmWindowA_80191D08(RoomEnt *obj);
extern void RoomLib_ArmWindowA_80191D14(RoomEnt *obj);
extern void RoomLib_ArmWindowA_80191D9C(RoomEnt *obj);
extern void RoomLib_ArmWindowA_80191E64(RoomEnt *obj);
extern void RoomLib_ArmWindowA_80192218(RoomEnt *obj);
extern void RoomLib_ArmWindowA_80192734(RoomEnt *obj);
extern void RoomLib_ArmWindowA_8019294C(RoomEnt *obj);
extern void RoomLib_ArmWindowA_80192E54(RoomEnt *obj);
extern void RoomLib_ArmWindowA_80192E5C(RoomEnt *obj);
extern void RoomLib_ArmWindowA_80195490(RoomEnt *obj);
extern void func_80191100(RoomEnt *obj);
extern void func_80191108(RoomEnt *obj);
extern void func_8019110C(RoomEnt *obj);
extern void func_80191114(RoomEnt *obj);
extern void func_80191118(RoomEnt *obj);
extern void func_8019111C(RoomEnt *obj);
extern void func_80191120(RoomEnt *obj);
extern void func_80191124(RoomEnt *obj);
extern void func_80191134(RoomEnt *obj);
extern void func_80191154(RoomEnt *obj);
extern void func_80191158(RoomEnt *obj);
extern void func_80191160(RoomEnt *obj);
extern void func_80191178(RoomEnt *obj);
extern void func_80191194(RoomEnt *obj);
extern void func_80191D9C(RoomEnt *obj);
extern void func_80192104(void);
extern void func_8019210C(RoomEnt *obj);
extern void func_80192118(RoomEnt *obj);
extern void func_801921A0(RoomEnt *obj);
extern void func_80192268(RoomEnt *obj);
extern void func_8019261C(RoomEnt *obj);
extern void func_80192B38(RoomEnt *obj);
extern void func_80192D50(RoomEnt *obj);
extern void func_80193258(RoomEnt *obj);
extern void func_80193260(RoomEnt *obj);
extern void func_80195894(RoomEnt *obj);
extern void RoomLib_ArmWindowB_80191814(RoomEnt *obj);
extern void func_801902C8(RoomEnt *obj);
extern void func_801902D4(RoomEnt *obj);
extern void func_80191814(RoomEnt *obj);
extern void func_80191820(RoomEnt *obj);
extern void RoomLib_ArmWindowB_8019181C(RoomEnt *obj);
extern void RoomLib_ArmWindowB_80191820(RoomEnt *obj);
extern void RoomLib_ArmWindowB_80191828(RoomEnt *obj);
extern void RoomLib_ArmWindowB_8019182C(RoomEnt *obj);
extern void RoomLib_ArmWindowB_80191830(RoomEnt *obj);
extern void RoomLib_ArmWindowB_80191834(RoomEnt *obj);
extern void RoomLib_ArmWindowB_80191838(RoomEnt *obj);
extern void RoomLib_ArmWindowB_80191848(RoomEnt *obj);
extern void RoomLib_ArmWindowB_80191868(RoomEnt *obj);
extern void RoomLib_ArmWindowB_8019186C(RoomEnt *obj);
extern void RoomLib_ArmWindowB_80191874(RoomEnt *obj);
extern void RoomLib_ArmWindowB_8019188C(RoomEnt *obj);
extern void RoomLib_ArmWindowB_801918A8(RoomEnt *obj);
extern void RoomLib_ArmWindowB_801924B0(RoomEnt *obj);
extern void RoomLib_ArmWindowB_80192818(RoomEnt *obj);
extern void RoomLib_ArmWindowB_80192820(RoomEnt *obj);
extern void RoomLib_ArmWindowB_8019282C(RoomEnt *obj);
extern void RoomLib_ArmWindowB_801928B4(RoomEnt *obj);
extern void RoomLib_ArmWindowB_8019297C(RoomEnt *obj);
extern void RoomLib_ArmWindowB_80192D30(RoomEnt *obj);
extern void RoomLib_ArmWindowB_8019324C(RoomEnt *obj);
extern void RoomLib_ArmWindowB_80193464(RoomEnt *obj);
extern void RoomLib_ArmWindowB_8019396C(RoomEnt *obj);
extern void RoomLib_ArmWindowB_80193974(RoomEnt *obj);
extern void RoomLib_ArmWindowB_80195FA8(RoomEnt *obj);
extern void func_80191D08(void);
extern void func_80191D10(void);
extern void func_80191D14(void);
extern void func_80191D1C(void);
extern void func_80191D20(void);
extern void func_80191D24(void);
extern void func_80191D28(void);
extern void func_80191D2C(void);
extern void func_80191D3C(void);
extern void func_80191D5C(void);
extern void func_80191D60(void);
extern void func_80191D68(void);
extern void func_80191D80(void);
extern void func_801929A4(void);
extern void func_80192D0C(void);
extern void func_80192D14(void);
extern void func_80192D20(void);
extern void func_80192DA8(void);
extern void func_80192E70(void);
extern void func_80193224(void);
extern void func_80193740(void);
extern void func_80193958(void);
extern void func_80193E60(void);
extern void func_80193E68(void);
extern void func_8019649C(void);
extern int RoomLib_Set4ClearSignal_801924D4(RoomEnt *o);

typedef struct RoomLibFxMatrixWords {
    int w0;
    int w1;
    int w2;
    int w3;
    int w4;
    int w5;
    int w6;
    int w7;
} RoomLibFxMatrixWords;

typedef struct RoomLibFxMatrixState {
    RoomLink *link;
    RoomLibFxMatrixWords matrix;
    void *asset;
} RoomLibFxMatrixState;

typedef struct RoomLibPacked8 {
    int lo;
    int hi;
} __attribute__((packed)) RoomLibPacked8;

extern void func_800C2B40(void *state);
extern void *func_8006DC18(int type);

#define ROOMLIB_JOIN_RAW(a, b) a##b
#define ROOMLIB_JOIN(a, b) ROOMLIB_JOIN_RAW(a, b)

#define ROOMLIB_INIT_EFFECT_PALETTE_B(name, colorA, colorB) \
    extern unsigned char ROOMLIB_JOIN(colorA, 0); \
    extern unsigned char ROOMLIB_JOIN(colorA, 1); \
    extern unsigned char ROOMLIB_JOIN(colorA, 2); \
    extern unsigned char ROOMLIB_JOIN(colorA, 4); \
    extern unsigned char ROOMLIB_JOIN(colorA, 5); \
    extern unsigned char ROOMLIB_JOIN(colorA, 6); \
    extern short ROOMLIB_JOIN(colorA, 8); \
    extern short ROOMLIB_JOIN(colorA, A); \
    extern unsigned char ROOMLIB_JOIN(colorB, 0); \
    extern unsigned char ROOMLIB_JOIN(colorB, 1); \
    extern unsigned char ROOMLIB_JOIN(colorB, 2); \
    extern unsigned char ROOMLIB_JOIN(colorB, 4); \
    extern unsigned char ROOMLIB_JOIN(colorB, 5); \
    extern unsigned char ROOMLIB_JOIN(colorB, 6); \
    extern short ROOMLIB_JOIN(colorB, 8); \
    extern short ROOMLIB_JOIN(colorB, A); \
    void name(char *obj, void *unused, char *state) { \
        char *owner; \
        RoomLibFxMatrixWords *words; \
        func_800C2B40(state); \
        *(void **)(state + 0x24) = func_8006DC18(0xB); \
        owner = *(char **)(obj + 8); \
        *(char **)state = owner; \
        words = *(RoomLibFxMatrixWords **)(owner + 0x238); \
        *(RoomLibFxMatrixWords *)(state + 4) = *words; \
        *(short *)(state + 0x28) = 0x28; \
        *(short *)(state + 0x2A) = 0; \
        *(short *)(state + 0x2C) = 0; \
        ROOMLIB_JOIN(colorB, 4) = 4; \
        ROOMLIB_JOIN(colorB, 5) = 1; \
        ROOMLIB_JOIN(colorB, 8) = 0; \
        ROOMLIB_JOIN(colorB, A) = 0x80; \
        ROOMLIB_JOIN(colorB, 0) = 0x80; \
        ROOMLIB_JOIN(colorB, 1) = 0x80; \
        ROOMLIB_JOIN(colorB, 2) = 0x80; \
        ROOMLIB_JOIN(colorB, 6) = 0; \
        ROOMLIB_JOIN(colorA, 4) = 8; \
        ROOMLIB_JOIN(colorA, 5) = 2; \
        ROOMLIB_JOIN(colorA, 8) = 0; \
        ROOMLIB_JOIN(colorA, A) = 0x30; \
        ROOMLIB_JOIN(colorA, 0) = 0x80; \
        ROOMLIB_JOIN(colorA, 1) = 0x80; \
        ROOMLIB_JOIN(colorA, 2) = 0x80; \
        ROOMLIB_JOIN(colorA, 6) = 0; \
    }

extern void RoomLib_FxNotify(RoomLink *l, struct RoomSub *s, int scratch);
extern void RoomLib_FxNotify2(RoomLink *l, struct RoomSub *s);
extern void func_800DFE94(void *a0, void *a1, void *a2);
extern int func_800DFC80(int *lhs, int *rhs);
extern int func_80079FB4(int x, int z);
extern void func_800DFB20(void *state);



/* steer entity toward FX target while active, otherwise settle toward current heading */
#define ROOMLIB_FX_NOTIFY(name) \
    void name(RoomLink *arg0, struct RoomSub *arg1, int scratch) { \
        char *ent = (char *)arg0; \
        char *rec = (char *)arg1; \
        if (*(short *)(rec + 0xA2) > 0) { \
            int *dst = (int *)(rec + 0x60); \
            char *v = *(char **)(rec + 0x84); \
            if (v != 0) { \
                dst[0] = *(int *)(v + 0x28); \
                dst[2] = *(int *)(v + 0x30); \
            } \
            *(short *)(ent + 0x3A) = FieldEng_TurnToward( \
                *(short *)(ent + 0x3A), \
                (short)FieldEng_VecToAngle(dst, (int *)(ent + 0x28)), \
                *(short *)(rec + 0xA2)) & 0xFFF; \
            if ((*(unsigned char *)(rec + 0xA9) != 0) \
                && (0x07FFFFFF < *(int *)(rec + 0x98))) { \
                *(short *)(rec + 0xA2) = 0; \
                *(short *)(rec + 0xA4) = 0; \
            } \
        } else { \
            func_800DFE94(ent + 0x28, ent + 0x40, ent + 0x38); \
            *(short *)(ent + 0x3A) = FieldEng_TurnToward( \
                *(short *)(ent + 0x3A), \
                *(short *)(ent + 0x3A), \
                *(short *)(rec + 0xA4)) & 0xFFF; \
        } \
    }



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

/* Move the field actor by a local, matrix-rotated X/Z step. */
#define ROOMLIB_MOVE_ACTOR_LOCAL(name, finish) \
    void name(RoomEnt *o) { \
        int height; \
        if (RW8(D_8009D254, 0xE) >= 4) { \
            finish(o); \
        } else if (RW32(D_8009D254, 0x98) & 0xC0000) { \
            RW32(D_8009D254, 0x98) &= 0xFFF3FFFF; \
            finish(o); \
        } else { \
            register volatile short *scratch asm("$3") = (volatile short *)0x1F800000; \
            scratch[0] = 0; \
            scratch[1] = 0; \
            scratch[2] = o->pos[0] >> 12; \
            gte_ldrotmatrix(o->mat); \
            gte_ldv0((void *)scratch); \
            gte_mvmva(); \
            gte_stmac((void *)&scratch[4]); \
            RW32(D_8009D254, 0x28) += *(volatile int *)&scratch[4] << 12; \
            RW32(D_8009D254, 0x30) += *(volatile int *)&scratch[8] << 12; \
            height = o->pos[0] - o->pos[1]; \
            o->pos[0] = height; \
            if (height < 0) { \
                finish(o); \
            } \
            if (o->h46 != 0) { \
                RW16(D_8009D254, 0x3A) = FieldEng_TurnToward( \
                    RW16(D_8009D254, 0x3A), o->h48, o->h46); \
            } \
        } \
    }

/* Build a yaw matrix and apply the scripted anchor and local motion vectors. */
#define ROOMLIB_ROTATE_MOTION(name) \
    void name(RoomLibMotionState *state, RoomLibMotionWork *work) { \
        int angle; \
        int *entry; \
        int *table; \
        int height; \
        int lo; \
        int hi; \
        if (state->target != 0) { \
            state->position[0] = state->target->pos[0]; \
            state->position[1] = state->target->pos[1]; \
            state->position[2] = state->target->pos[2]; \
        } \
        if (state->mode.word != 0) { \
            angle = FieldEng_VecToAngle(state->anchor, state->position); \
            angle &= 0xFFF; \
            table = (int *)D_800966EC; \
            entry = (int *)((char *)table + (angle << 2)); \
            hi = *(short *)((char *)entry + 2); \
            work->matrix[1] = 0; \
            work->matrix[0] = hi; \
            lo = *entry; \
            work->matrix[3] = 0; \
            work->matrix[5] = 0; \
            work->matrix[7] = 0; \
            work->input[0] = 0; \
            work->input[1] = 0; \
            work->matrix[2] = lo; \
            work->matrix[4] = 0x1000; \
            work->matrix[8] = work->matrix[0]; \
            work->matrix[6] = -(unsigned short)work->matrix[2]; \
            height = state->mode.half.height; \
            work->input[2] = height; \
            gte_ldrotmatrix(work->matrix); \
            gte_ldv0(work->input); \
            gte_mvmva(); \
            gte_stmac(work->rotated); \
            state->position[0] = state->anchor[0] + (work->rotated[0] << 16); \
            state->position[2] = state->anchor[2] + (work->rotated[2] << 16); \
        } \
        angle = FieldEng_VecToAngle(state->anchor, state->position); \
        angle &= 0xFFF; \
        table = (int *)D_800966EC; \
        entry = (int *)((char *)table + (angle << 2)); \
        hi = *(short *)((char *)entry + 2); \
        work->matrix[1] = 0; \
        work->matrix[0] = hi; \
        lo = *entry; \
        work->matrix[3] = 0; \
        work->matrix[5] = 0; \
        work->matrix[7] = 0; \
        work->matrix[2] = lo; \
        work->matrix[4] = 0x1000; \
        work->matrix[8] = work->matrix[0]; \
        work->matrix[6] = -(unsigned short)work->matrix[2]; \
        gte_ldrotmatrix(work->matrix); \
        gte_ldv0(state->localStep); \
        gte_mvmva(); \
        gte_stmac(work->rotated); \
        state->position[0] += work->rotated[0] << 16; \
        state->position[1] += work->rotated[1] << 16; \
        state->position[2] += work->rotated[2] << 16; \
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
#define ROOMLIB_ARG_DISPATCH_REARM(name, rearm) \
    int name(RoomEnt *o, int arg1, unsigned int op, int arg3, int sp10, int sp14) { \
        if (op == 0xA) { \
            goto case10; \
        } \
        if (op >= 0xB) { \
            goto high; \
        } \
        if (op == 4) { \
            goto case4; \
        } \
        goto done; \
    high: \
        if (op == 0x19) { \
            goto case25; \
        } \
        if (op == 0x1C) { \
            goto case28; \
        } \
        goto done; \
    case25: \
        if (arg1 != 1) { \
            goto done; \
        } \
        o->sub.signal = (int *)arg3; \
        *(int *)arg3 = arg1; \
        goto done; \
    case4: \
        o->pos[0] = arg3; \
        o->pos[1] = sp10; \
        o->h44 = sp14; \
        goto done; \
    case28: \
        o->h48 = arg3; \
        o->h46 = sp10; \
        goto done; \
    case10: \
        o->t16 = arg3; \
        o->sub.cb = (void (*)(void))rearm; \
    done: \
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

/*
 * Target-relative, phase-based movement.  The only non-C operations are the
 * named GTE macros that load a yaw matrix, transform one vector, and read it
 * back; all state and interpolation logic remains ordinary C.
 */
#define ROOMLIB_HANDLER_E(name, reset, steer, rotTable) \
    void name(RoomEnt *o) { \
        RoomLink *link = o->link; \
        RoomLink *targetLink; \
        RoomLibHandlerEState *state = (RoomLibHandlerEState *)((char *)o + 0xC); \
        volatile short *scratch = (volatile short *)0x1F800000; \
        int remain; \
        int scale; \
        int angle; \
        int rotWord; \
        int localX; \
        int localZ; \
        int rotY; \
        short rotX; \
        short negRotY; \
        short *rotBase; \
        short *rot; \
        if (o->t1A == 0) { \
            RW16(o, 0x86) = 1; \
            o->t1A = 1; \
            RW32(link, 0x98) &= 0xFFF3FFFF; \
            if (o->active != 0 && link->target != 0) { \
                link->target->flags |= 0x40000000; \
            } \
            state->start[0] = link->pos[0]; \
            state->start[1] = link->pos[1]; \
            state->start[2] = link->pos[2]; \
            targetLink = state->targetLink; \
            if (targetLink != 0) { \
                state->target[0] = targetLink->pos[0]; \
                state->target[1] = targetLink->pos[1]; \
                state->target[2] = targetLink->pos[2]; \
            } \
            angle = FieldEng_VecToAngle(state->start, state->target) & 0xFFF; \
            { \
                rotBase = (short *)(rotTable); \
                rot = rotBase + (angle << 1); \
                rotY = rot[1]; \
                scratch[21] = 0; \
                scratch[20] = rotY; \
                rotWord = *(int *)rot; \
                rotX = scratch[20]; \
            } \
            scratch[23] = 0; \
            scratch[25] = 0; \
            scratch[27] = 0; \
            scratch[22] = rotWord; \
            negRotY = scratch[22]; \
            scratch[24] = 0x1000; \
            scratch[28] = rotX; \
            scratch[26] = -negRotY; \
            localX = state->localOffset[0]; \
            scratch[1] = 0; \
            scratch[0] = localX >> 12; \
            localZ = state->localOffset[2]; \
            do { \
                scratch[2] = localZ >> 12; \
                gte_ldrotmatrix((void *)((int)scratch | 0x28)); \
            } while (0); \
            gte_ldv0((void *)scratch); \
            gte_mvmva(); \
            gte_stmac((void *)&scratch[4]); \
            state->target[0] += *(int *)&scratch[4] << 12; \
            state->target[1] += *(int *)&scratch[6] << 12; \
            state->target[2] += *(int *)&scratch[8] << 12; \
            if (state->duration == 0) { \
                state->duration = ((func_800DFC80(state->start, state->target) << 16) / state->speed) + 1; \
            } \
            state->delta[0] = (state->target[0] - state->start[0] + 0x800) >> 12; \
            state->delta[1] = (state->target[1] - state->start[1] + 0x800) >> 12; \
            state->delta[2] = (state->target[2] - state->start[2] + 0x800) >> 12; \
            state->heading = func_80079FB4(state->delta[0], state->delta[2]); \
        } \
        scratch[52] = 0; \
        remain = state->phase - state->phaseFrame[state->frame]; \
        if (remain < 0) { \
            remain = 0; \
        } \
        if (state->duration < remain) { \
            remain = state->duration; \
        } \
        if (state->eased != 0) { \
            scale = 0x1000 - D_800966EE[(((remain << 11) / state->duration) & 0xFFF) << 1]; \
            link->pos[0] = state->start[0] + (((state->delta[0] * scale + 0x1000) >> 13) << 12); \
            if (state->lockY == 0) { \
                link->pos[1] = state->start[1] + (((state->delta[1] * scale + 0x1000) >> 13) << 12); \
            } \
            link->pos[2] = state->start[2] + (((state->delta[2] * scale + 0x1000) >> 13) << 12); \
        } else { \
            link->pos[0] = state->start[0] + ((state->delta[0] * remain / state->duration) << 12); \
            if (state->lockY == 0) { \
                link->pos[1] = state->start[1] + ((state->delta[1] * remain / state->duration) << 12); \
            } \
            link->pos[2] = state->start[2] + ((state->delta[2] * remain / state->duration) << 12); \
        } \
        if (state->copyPosition != 0) { \
            link->posMirror[0] = link->pos[0]; \
            link->posMirror[1] = link->pos[1]; \
            link->posMirror[2] = link->pos[2]; \
            link->posMirror[3] = link->pos[3]; \
        } \
        if (remain >= state->duration && state->frame >= state->frameLimit) { \
            reset(o); \
        } else { \
            state->frame++; \
            if (state->frameLimit < state->frame) { \
                state->frame = 0; \
                state->phase++; \
            } \
        } \
        steer(link, state); \
        func_800DFB20(o); \
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

/* same table registration pattern, but only while the field engine is in status 3 */

/* register unless a low-variant linked target has its room-local AC gate clear */

/* plant the room table pointer into the engine slot */
#define ROOMLIB_PLANT_TABLE(name, table) \
    int name(void) { \
        *FieldEng_GetSlot() = table; \
        return 0; \
    }

/* stash three args into room slots; returns the first slot */
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

/* Dialog animation state consumed by the shared RoomLib_DlgBlob handler.
 * Its first mode integrates the three velocity halfwords into x/y/z and
 * accelerates velocityY by two each tick. */
typedef struct RoomDlgAnimState {
    short x;                     /* 0x00 */
    short y;                     /* 0x02 */
    short z;                     /* 0x04 */
    short field_06;              /* 0x06 */
    short velocityX;             /* 0x08 */
    short velocityY;             /* 0x0A */
    short velocityZ;             /* 0x0C */
    short field_0E;              /* 0x0E */
    short state;                 /* 0x10 */
    short timer;                 /* 0x12 */
} RoomDlgAnimState;

typedef struct RoomDlgAnimParams {
    char pad00[0x4];
    int scale;                   /* 0x04 */
} RoomDlgAnimParams;

typedef int (*RoomDlgCallback)(int mode, RoomDlgAnimState *state,
                               RoomDlgAnimParams *params);

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
extern int func_8018FEF8(RoomEnt *o);
extern void func_801909E0(RoomEnt *o);

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

typedef struct RoomTimer0 {
    char pad0[0x4];
    unsigned short h4;            /* 0x04: reload value */
    short h6;                     /* 0x06: countdown */
    short h8;                     /* 0x08: fire request */
} RoomTimer0;

typedef struct RoomTimer2 {
    char pad0[0x28];
    unsigned short h28;           /* 0x28: reload value */
    short h2A;                    /* 0x2A: countdown */
    short h2C;                    /* 0x2C: fire request */
} RoomTimer2;

extern int func_800C6C18();
extern int func_800C2B68();

#define ROOMLIB_PARTICLE_TICK_A(name) \
    void name(RoomEnt *o, unsigned char *state, char *sys) { \
        struct { short a; short pad; short b; } volatile saved; \
        char *clock = (char *)func_800C2B50(); \
        int tmp = RW16(o->link, 0x2A); \
        saved.a = tmp; \
        tmp = RW16(o->link, 0x32); \
        saved.b = tmp; \
        if (RW16(state, 2) < 0x20) { \
            RWU16(sys, 0x138) = RWU16(sys, 0x138) + RWU16(sys, 0x13C); \
            RWU16(sys, 0x13C) = RWU16(sys, 0x13C) + 7; \
            if (RW16(state, 2) < 0x20 && RW16(sys, 0x13A) >= 5) { \
                RW16(sys, 0x13A) = RW16(sys, 0x13A) - 4; \
            } \
        } \
        if (RW16(state, 2) == 0x10) { \
            RW16(clock, 0x2C) = 1; \
        } \
        if (RW16(state, 2) == 0x20) { \
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
    char pad0[0x8];
    short h8;                     /* 0x08 */
    short hA;                     /* 0x0A */
    short hC;                     /* 0x0C: current tick */
    char padE[0x2];
    unsigned char renderOwner;   /* 0x10 */
} RoomClock;

extern RoomClock *func_800C2B50();
extern int func_800C6B90(void *position, int radius);
extern void func_800C2EAC(u8 owner);
extern void func_800C2FF0(s32 width, s32 height);
extern void func_800C3098(s32 depth);
extern void func_800C3238(s32 mode);
extern void func_800C42A4(void *packet, RoomSpriteMatrix *matrix, s32 mode);
extern void func_80078CC4(RoomSpriteMatrix *matrix, RoomFxVec4 *scale);
extern int func_80071A54(void);

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

#define ROOMLIB_UPDATE_GROUND_PULSE(name) \
    void name(int unused, RoomStatePair *state, RoomFxParams *fx) { \
        RoomClock *clock = func_800C2B50(); \
        if (clock->hC - 0x3C < state->h2) { \
            if (fx->hA >= 5) { \
                fx->hA -= 4; \
            } \
            if (clock->hC - 0x1E < state->h2 && \
                fx->h8 > clock->hA * 6) { \
                fx->h8 -= clock->hA * 6; \
            } \
        } else { \
            fx->h8 += clock->hA; \
        } \
        if (func_800C6B90(fx, fx->h8 >> 3) != 0) { \
            clock->h8 = 1; \
        } \
        if (state->h2 == clock->hC) { \
            state->b1 = 2; \
        } \
    }

typedef struct RoomPartVec {
    unsigned short x;
    unsigned short y;
    unsigned short z;
    short pad6;
} RoomPartVec;

typedef struct RoomSpritePacket {
    unsigned char header[0xA];
    unsigned short depth;
} RoomSpritePacket;

extern short D_800942EC;
typedef struct RoomParticleRenderState {
    RoomSpriteMatrix matrix;      /* 0x00 */
    RoomPartVec position[6];      /* 0x20 */
    RoomPartVec velocity[6];      /* 0x50 */
    char pad80[0xC];
    unsigned short depth[6];      /* 0x8C */
    short active[6];              /* 0x98 */
    short liveCount;              /* 0xA4 */
} RoomParticleRenderState;

#define ROOMLIB_INIT_PARTICLE_RENDER_STATE(name, scaleObj) \
    void name(int unused, void *state, RoomParticleRenderState *fx) { \
        extern RoomFxVec4 scaleObj; \
        RoomFxVec4 scale; \
        unsigned int i; \
        fx->matrix.m[2][2] = 0x1000; \
        fx->matrix.m[1][1] = 0x1000; \
        fx->matrix.m[0][0] = 0x1000; \
        fx->matrix.t[2] = 0; \
        fx->matrix.t[1] = 0; \
        fx->matrix.t[0] = 0; \
        fx->matrix.m[2][1] = 0; \
        fx->matrix.m[2][0] = 0; \
        fx->matrix.m[1][2] = 0; \
        fx->matrix.m[1][0] = 0; \
        fx->matrix.m[0][2] = 0; \
        fx->matrix.m[0][1] = 0; \
        scale = scaleObj; \
        func_80078CC4(&fx->matrix, &scale); \
        fx->liveCount = 6; \
        for (i = 0; i < 6; i++) { \
            fx->active[i] = 1; \
            fx->depth[i] = 0x80; \
            fx->velocity[i].x = func_80071A54() % 31 - 15; \
            fx->velocity[i].y = -(func_80071A54() % 100 + 50); \
            fx->velocity[i].z = func_80071A54() % 31 - 15; \
        } \
    }

#define ROOMLIB_RENDER_PARTICLE_DEPTHS(name, packetObj, frameY) \
    void name(int unused, void *state, RoomParticleRenderState *fx) { \
        extern RoomSpritePacket packetObj; \
        RoomClock *clock = func_800C2B50(); \
        unsigned int i; \
        func_800C2EAC(clock->renderOwner); \
        func_800C2FF0(0x10, 0x10); \
        func_800C3098(0x10); \
        func_800C3238(1); \
        for (i = 0; i < 6; i++) { \
            if (fx->active[i] == 1) { \
                fx->matrix.t[0] = (short)fx->position[i].x; \
                fx->matrix.t[1] = (short)fx->position[i].y; \
                fx->matrix.t[2] = (short)fx->position[i].z; \
                packetObj.depth = fx->depth[i]; \
                func_800C42A4(&packetObj, &fx->matrix, 1); \
                packetObj.depth = (short)fx->depth[i] >> 2; \
                fx->matrix.t[1] = frameY; \
                func_800C42A4(&packetObj, &fx->matrix, 1); \
            } \
        } \
    }

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

#define ROOMLIB_HANDLER_G(name, next_handler, set4_clear_signal) \
    void name(RoomEnt *o) { \
        RoomEnt *self = o; \
        RoomLink *src = (RoomLink *)RW32(self, 0x1C); \
        RoomLink *dst = self->link; \
        dst->pos[0] = RW32(src->p238, 0x94) << 16; \
        dst->pos[1] = RW32(src->p238, 0x98) << 16; \
        dst->pos[2] = RW32(src->p238, 0x9C) << 16; \
        *(RoomLibPacked8 *)((char *)dst + 0x38) = *(RoomLibPacked8 *)((char *)src + 0x38); \
        if ((src->variant == 7) && (src->winLo >= 3) && (src->winHi < 3)) { \
            self->sub.cb = next_handler; \
        } \
        if (src->target != 0) { \
            if (func_8003010C(src, 0x2C) <= 0) { \
                func_80030220(dst, 0x2D, 0); \
                func_80030220(dst, 0x2C, 0); \
                func_80030220(dst, 0x5F, 0); \
                set4_clear_signal(self); \
            } \
        } else { \
            set4_clear_signal(self); \
        } \
    }

#endif
