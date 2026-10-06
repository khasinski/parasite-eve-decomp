/* Shared room-script library. Every room links its own copy; instance
 * functions are stamped per room/offset with these body macros. */
#ifndef ROOM_LIB_H
#define ROOM_LIB_H

#include "../../../include/common.h"
#include "pe1/field_anim_callback_list.h"
#include "../../../include/pe1/gte.h"
#include "../../../include/pe1/room_fx.h"
#include "../../../include/pe1/field_script_context.h"
#include "../../../include/pe1/room_particle_state.h"
#include "../../../include/pe1/render_object.h"
#include "../../../include/pe1/field_sprite_state.h"
#include "pe1/room_floor.h"

typedef struct RoomObj {
    char pad0[0xC];
    void (*callback)(void);
} RoomObj;

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

/* Locals for the room rotation-table lookups. These used to carry register
 * pins; stock allocation now places them where retail does. */
#define ROOMLIB_ROT_ENTRY_DECL \
    int *entry; \
    int *base

#define ROOMLIB_V0_PTR_DECL(name) void *name
#define ROOMLIB_V1_INT_DECL(name) int name
#define ROOMLIB_A0_INT_DECL(name) int name
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


struct RoomEnt;
extern void RoomLib_HandlerA(struct RoomEnt *obj);
extern void RoomLib_HandlerB(struct RoomEnt *obj);
extern void RoomLib_HandlerC(struct RoomEnt *obj);
extern void RoomLib_HandlerD(struct RoomEnt *obj);
extern void RoomLib_HandlerE(struct RoomEnt *obj);
extern void RoomLib_HandlerF();
extern int FieldEng_VecToAngle(int *vec, int *ref);
extern int FieldEng_TurnToward(short cur, short target, short rate);
extern char *RoomMain_ActorPtr;
extern int RoomMain_RotTable[];
/* Field object status: 0 no model, 1 model without data, 2 inactive,
 * 3 active, 4 disabled object, 5 the player. */
extern int FieldEng_GetStatus(void *object);
extern void FieldEng_Spawn6(int a, int b, int c, int d, int e, int f);
extern void FieldEng_Register(void *o, void *table);
extern int func_800C251C(void *o, void *table);
extern int func_800C2758(void *o, void *tableA, void *tableB);
extern void **FieldEng_GetSlot(void);
extern int func_8003010C(void *o, int arg);
extern void func_80030220(void *o, int arg, int value);
extern int func_80192BFC();

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

/* Four lanes of three-component values with increments and a shared timer. */
typedef struct RoomLibStepRecords {
    char pad00[0x30];
    unsigned short value[4][4];  /* 0x30: 8-byte lane stride */
    unsigned short increment[4][4]; /* 0x50 */
    char pad70[0x8];
    unsigned short timer[4];     /* 0x78 */
} RoomLibStepRecords;

PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomLibStepRecords, value) == 0x30,
                  room_lib_step_values_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomLibStepRecords, increment) == 0x50,
                  room_lib_step_increments_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomLibStepRecords, timer) == 0x78,
                  room_lib_step_timer_offset);
PE1_STATIC_ASSERT(sizeof(RoomLibStepRecords) == 0x80,
                  room_lib_step_records_size);

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

extern short D_800966EE[];
extern short D_800966EC[][2];
extern char *D_8009D254;
struct FieldActorNode;
extern struct FieldActorNode *D_8009D20C;
extern int RoomLib_ResetAndSignal_801914B0(RoomEnt *obj);
extern int RoomLib_Set4ClearSignal_80192510(RoomEnt *obj);
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
extern void RoomLib_ArmWindowA_80190D48(RoomEnt *obj);
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
extern void RoomLib_ArmWindowB_80191860(RoomEnt *obj);
extern void func_801902C8(RoomEnt *obj);
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
extern void func_80191D08();
extern void func_80191D10(void);
extern void func_80191D54(void);
extern void func_8019114C(RoomEnt *obj);
extern void func_80191D14(void);
extern void func_80191D1C(void);
extern void func_80191D20(void);
extern void func_80191D24(void);
extern void func_80191D28(void);
extern void func_80191D2C(void);
extern void func_80191D3C(void);
extern void func_80191D5C(RoomEnt *obj);
extern void func_80191D60(void);
extern void func_80191D68(RoomEnt *obj);
extern void func_80191D80(void);
extern void func_801929A4(RoomEnt *obj);
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
extern void func_8019649C(struct RoomEnt *obj);
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

extern void *func_8006DC18(int type);

/* Each room that links RoomLib_TwelveElementEffect defines the two packet
 * templates and the sprite table in its data. */
extern RoomFxSpritePacket RoomLib_TwelveEffectPrimaryPacket;
extern RoomFxSpritePacket RoomLib_TwelveEffectSecondaryPacket;
extern unsigned char RoomLib_TwelveEffectTable[];

#define ROOMLIB_JOIN_RAW(a, b) a##b
#define ROOMLIB_JOIN(a, b) ROOMLIB_JOIN_RAW(a, b)

extern void RoomLib_FxNotify(RoomLink *l, struct RoomSub *s, int scratch);
extern void func_800DFE94(void *a0, void *a1, void *a2);
extern int func_800DFC80(int *lhs, int *rhs);
extern int ratan2(int x, int z);
extern void func_800DFB20(void *state);


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
            volatile short *scratch = (volatile short *)0x1F800000; \
            scratch[0] = 0; \
            scratch[1] = 0; \
            scratch[2] = o->pos[0] >> 12; \
            gte_ldrotmatrix(o->mat); \
            gte_ldv0((void *)scratch); \
            gte_mvmva(); \
            gte_stmac((void *)&scratch[4]); \
            RW32(D_8009D254, 0x28) += *(int *)&scratch[4] << 12; \
            RW32(D_8009D254, 0x30) += *(int *)&scratch[8] << 12; \
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


/* argument parser variant whose comparison value is stored as the fallback handler */
#define ROOMLIB_ARG_DISPATCH_REARM_FALLBACK(name, rearm) \
    int name(RoomEnt *o, int arg1, unsigned int op, int arg3, int sp10, int sp14) { \
        int value = 0xA; \
        if (op == value) { \
            goto case10; \
        } \
        value = op < 0xB; \
        if (value == 0) { \
            goto high; \
        } \
        value = 4; \
        if (op == value) { \
            goto case4; \
        } \
        goto store; \
    high: \
        value = 0x19; \
        if (op == value) { \
            goto case25; \
        } \
        value = 0x1C; \
        if (op == value) { \
            goto case28; \
        } \
        goto store; \
    case25: \
        if (arg1 != 1) { \
            goto done; \
        } \
        o->sub.signal = (int *)arg3; \
        *(int *)arg3 = arg1; \
        value = 1; \
        goto store; \
    case4: \
        o->pos[0] = arg3; \
        o->pos[1] = sp10; \
        o->h44 = sp14; \
        goto store; \
    case28: \
        o->h48 = arg3; \
        o->h46 = sp10; \
        goto store; \
    case10: \
        value = (int)rearm; \
    store: \
        *(volatile unsigned char *)&o->t16 = arg3; \
        o->sub.cb = (void (*)(void))value; \
    done: \
        return 0; \
    }


extern char RoomLib_TableA[];
extern char RoomLib_TableB[];

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
        if ((unsigned int)FieldEng_GetStatus(o) >= 2) { \
            RoomLinkByte *tgt = o->link->target; \
            *(int *)tgt &= 0xC0FFFFFF; \
            *o->link->target->state = 4; \
        } \
        return 0; \
    }

/* register this entity's table with the field engine when active */
#define ROOMLIB_REGISTER_TABLE(name, table) \
    int name(void *o) { \
        if ((unsigned int)FieldEng_GetStatus(o) >= 2) { \
            FieldEng_Register(o, table); \
        } \
        return 0; \
    }

/* registration method that registers the draw list whatever the field
 * engine's status */
#define ROOMLIB_REGISTER_TABLE_ANY(name, table) \
    int name(void *o) { \
        FieldEng_Register(o, table); \
        return 0; \
    }

/* start method that acts whatever the field engine's status: register the
 * update handlers and spawn from the init list and the spawn layout,
 * closing the module with `close` when either call fails */
#define ROOMLIB_START_ANY(name, update, init, layout, close) \
    int name(void *o) { \
        if ((func_800C251C(o, update) | func_800C2758(o, init, layout)) == -1) { \
            close(o); \
        } \
        return 0; \
    }

/* start method that acts only while the field engine runs the object
 * (status 3): register the update handlers and spawn from the init list and
 * the spawn layout, closing the module with `close` when either call fails
 * or the object is not running */
#define ROOMLIB_START_AT3(name, update, init, layout, close) \
    int name(void *o) { \
        int result; \
        if (FieldEng_GetStatus(o) == 3) { \
            result = func_800C251C(o, update); \
            result |= func_800C2758(o, init, layout); \
        } else { \
            result = -1; \
        } \
        if (result == -1) { \
            close(o); \
        } \
        return 0; \
    }

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

typedef struct RoomSlotRec {
    short h0;
    short h2;                     /* stamped with the global frame counter */
    short h4;
    short pad6;
    int w8;
    int wC;
} RoomSlotRec;


/* The room's slot table (RoomLib_SlotSet.c), named in each room's symbol
 * file. */
extern RoomSlotRec RoomLib_SlotTable[];
RoomSlotRec *RoomLib_SlotSet(int mode, int idx, int a, int b);

typedef struct RoomBlob8 {
    char b[8];
} RoomBlob8;

typedef struct RoomMsgSub {
    short h0;
    short h2;
    short h4;
} RoomMsgSub;

typedef struct RoomMsg {
    short h0;
    short h2;
    short h4;
    short h6;
    RoomMsgSub sub;               /* 0x08 */
} RoomMsg;

/* Dropped flare particle state as seen by the dispatcher; the handler at
 * RoomLib_DlgBlob is RoomEffect_DroppedFlareParticle (room_flare.h).
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
extern int func_800CFAA8();
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

/* Field engine state query used by the actor class update handlers. */
extern int func_800DFB78();

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
        struct { short a; short pad; short b; } saved; \
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

extern int func_800C6B90(void *position, int radius);
#include "pe1/gte_types.h"
#include "pe1/field_glow_sprite.h"
extern int func_80071A54(void);


#define ROOMLIB_REGISTER_TABLE_AT3(name, table) \
    int name(void *o) { \
        if (FieldEng_GetStatus(o) == 3) { \
            FieldEng_Register(o, table); \
        } \
        return 0; \
    }

/* The room library's actor classes (RoomLib_ActorClasses.c, and
 * RoomLib_FloorWalkerClass.c in the rooms that link the longer library).
 * A room's class table lists seven methods per class: three no-ops (slots
 * 0, 3 and 6), Init, Configure, Update and Release; the remaining functions
 * are the class's private states. */
struct RoomLibMotionState;
struct RoomLibMotionWork;
int RoomLib_HandlerDNop0(void);
int RoomLib_InitHandlerD(RoomEnt *o);
int RoomLib_ConfigureHandlerD(RoomEnt *o, int query, unsigned int op,
                              int arg0, int arg1, int arg2);
int RoomLib_HandlerDNop3(void);
int RoomLib_UpdateHandlerD(RoomEnt *o);
void RoomLib_ArmHandlerD(RoomEnt *o);
void RoomLib_HandlerDTransformTarget(struct RoomLibMotionState *state,
                                     struct RoomLibMotionWork *work);
int RoomLib_ReleaseHandlerD(RoomEnt *o);
int RoomLib_HandlerDNop6(void);
int RoomLib_HandlerENop0(void);
int RoomLib_InitHandlerE(RoomEnt *o);
int RoomLib_ConfigureHandlerE(RoomEnt *o, int query, unsigned int op,
                              int arg0, int arg1, int arg2);
int RoomLib_HandlerENop3(void);
int RoomLib_UpdateHandlerE(RoomEnt *o);
void RoomLib_ArmHandlerE(RoomEnt *o);
void RoomLib_HandlerESteerToward(char *entity, char *state);
int RoomLib_ReleaseHandlerE(RoomEnt *o);
int RoomLib_HandlerENop6(void);
int RoomLib_HandlerBNop0(void);
int RoomLib_InitHandlerB(RoomEnt *o);
int RoomLib_ConfigureHandlerB(RoomEnt *o, int query, unsigned int op,
                              int arg0, int arg1, int arg2);
int RoomLib_HandlerBNop3(void);
int RoomLib_UpdateHandlerB(RoomObj *obj);
void RoomLib_ArmHandlerB(RoomEnt *o);
void RoomLib_HandlerBPhase(RoomEnt *obj);
void RoomLib_AdvanceArcToTarget(RoomEnt *obj);
int RoomLib_ReleaseHandlerB(RoomEnt *o);
int RoomLib_HandlerBNop6(void);
int RoomLib_HandlerCNop0(void);
int RoomLib_InitHandlerC(RoomEnt *o);
int RoomLib_ConfigureHandlerC(RoomEnt *o, int query, unsigned int op,
                              int arg0, int arg1, int arg2);
int RoomLib_HandlerCNop3(void);
int RoomLib_UpdateHandlerC(RoomObj *obj);
void RoomLib_ArmHandlerC(RoomEnt *o);
void RoomLib_HandlerCPhase(void);
void RoomLib_AdvanceArcToTargetY(RoomEnt *obj);
int RoomLib_ReleaseHandlerC(RoomEnt *o);
int RoomLib_HandlerCNop6(void);
int RoomLib_HandlerANop0(void);
int RoomLib_InitHandlerA(RoomEnt *o);
int RoomLib_ConfigureHandlerA(RoomEnt *o, int arg1, unsigned int op,
                              int arg3, int sp10, int sp14);
int RoomLib_HandlerANop3(void);
int RoomLib_UpdateHandlerA(RoomObj *obj);
void RoomLib_RearmHandlerA(RoomEnt *o);
int RoomLib_ReleaseHandlerA(RoomEnt *o);
int RoomLib_HandlerANop6(void);
struct RoomFloorWalkerObject;
int RoomLib_HandlerGNop0(void);
int RoomLib_InitHandlerG(RoomEnt *o);
int RoomLib_ConfigureHandlerG(RoomEnt *o, int mode, unsigned int op,
                              int arg0, int arg1, int arg2);
int RoomLib_HandlerGNop3(void);
void RoomLib_HandlerG(RoomEnt *o);
void RoomLib_HandlerGFloorWalker(struct RoomFloorWalkerObject *object);
int RoomLib_ReleaseHandlerG(RoomEnt *o);
int RoomLib_HandlerGNop6(void);

#endif
