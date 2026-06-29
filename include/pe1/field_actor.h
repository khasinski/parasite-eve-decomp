#ifndef PE1_FIELD_ACTOR_H
#define PE1_FIELD_ACTOR_H

typedef struct FieldActor {
    /* 0x000 */ unsigned char pad_000[0x04];
    /* 0x004 */ struct FieldActor *next;   /* entity list link (D_8009D20C walks this) */
    /* 0x008 */ unsigned char pad_008[0x04];
    /* 0x00C */ unsigned char type_id;     /* matched against arg id in attachment lookup */
    /* 0x00D */ unsigned char sub_id;
    /* 0x00E */ unsigned char mode;
    /* 0x00F */ unsigned char action;
    /* 0x010 */ unsigned char pad_010[0x02];
    /* 0x012 */ unsigned short anim_frame_target; /* clamped target anim frame (Task_SetEntityAnimFrame) */
    /* 0x014 */ unsigned char pad_014[0x02];
    /* 0x016 */ unsigned short anim_frame;        /* current anim frame; wait until == target (Task_WaitAnimFrameMatch) */
    /* 0x018 */ unsigned char pad_018[0x10];
    /* 0x028 */ int pos_x;
    /* 0x02C */ int pos_y;
    /* 0x030 */ int pos_z;
    /* 0x034 */ unsigned char pad_034[0x04];
    /* 0x038 */ unsigned short rot_x;
    /* 0x03A */ unsigned short rot_y;   /* facing angle (yaw); 12-bit, 0x1000 = 360deg */
    /* 0x03C */ unsigned short rot_z;
    /* 0x03E */ unsigned char pad_03E[0x02];
    /* 0x040 */ int base_x;             /* pre-move position (rollback target) */
    /* 0x044 */ int base_y;
    /* 0x048 */ int base_z;
    /* 0x04C */ int field_4c;           /* actor status bits (player's 0xC0 mask gates a scene flag check) */
    /* 0x050 */ unsigned short saved_rot_x;
    /* 0x052 */ unsigned short saved_rot_y;
    /* 0x054 */ unsigned short saved_rot_z;
    /* 0x056 */ unsigned char pad_056[0x02];
    /* 0x058 */ int delta_x;            /* extra per-frame displacement added after motion */
    /* 0x05C */ int delta_y;
    /* 0x060 */ int delta_z;
    /* 0x064 */ unsigned char pad_064[0x04];
    /* 0x068 */ int motion_x;
    /* 0x06C */ int motion_y;
    /* 0x070 */ int motion_z;
    /* 0x074 */ unsigned char pad_074[0x04];
    /* 0x078 */ int accel_x;   /* constant per-frame accel added to motion (Entity_IntegratePositionFull) */
    /* 0x07C */ int accel_y;
    /* 0x080 */ int accel_z;
    /* 0x084 */ unsigned char pad_084[0x04];
    /* 0x088 */ int gravity_x; /* gravity accel added to motion when flags & 2 */
    /* 0x08C */ int gravity_y;
    /* 0x090 */ int gravity_z;
    /* 0x094 */ unsigned char pad_094[0x04];
    /* 0x098 */ unsigned int flags;
    /* 0x09C */ int field_9c;                             /* script jump base (Task_JumpIfZero / Task_JumpToEntityOffset) */
    /* 0x0A0 */ struct FieldActorNode *task_node_lists[3]; /* 3 task-node list heads (Entity_MarkNodeFree) */
    /* 0x0AC */ unsigned char pad_0AC[0xE0];
    /* 0x18C */ struct FieldActor *parent; /* parent actor; child copies its pos/rot when flags & 0x400000 */
    /* 0x190 */ unsigned char pad_190[0x14];
    /* 0x1A4 */ int field_1a4;          /* rolled back from field_1a8 alongside pos */
    /* 0x1A8 */ int field_1a8;
    /* 0x1AC */ unsigned char pad_1AC[0x08];
    /* 0x1B4 */ unsigned char attachment[0x5C]; /* render attachment block copied by Entity_CopyAttachmentData */
    /* 0x210 */ short descriptor_value_a;
    /* 0x212 */ short descriptor_value_b;
} FieldActor;

typedef struct FieldActorState {
    /* 0x00 */ unsigned char pad_00[0x08];
    /* 0x08 */ int progress;
    /* 0x0C */ short amount;
    /* 0x0E */ unsigned short amount_mirror;
    /* 0x10 */ short actor_threshold;
    /* 0x12 */ unsigned char saved_actor_mode;
    /* 0x13 */ unsigned char pad_13;
    /* 0x14 */ unsigned char pad_14[0x08];
    /* 0x1C */ int count_limit;
    /* 0x20 */ int divisor_basis;
    /* 0x24 */ short actor_threshold_delta;
    /* 0x26 */ unsigned char pad_26[0x02];
    /* 0x28 */ int progress_target;
    /* 0x2C */ int progress_step;
    /* 0x30 */ int countdown;
    /* 0x34 */ int terminal_delay;
    /* 0x38 */ unsigned char transition_timers[0x10];
    /* 0x48 */ signed char recoil_steps;
    /* 0x49 */ unsigned char recoil_magnitude;
    /* 0x4A */ short recoil_angle;
    /* 0x4C */ unsigned int flags;
    /* 0x50 */ unsigned char number_desc_0[0x08];
    /* 0x58 */ unsigned char number_desc_1[0x08];
    /* 0x60 */ unsigned char coord_desc[0x08];
    /* 0x68 */ unsigned char pad_68[0x04];
    /* 0x6C */ void *substate;
} FieldActorState;
/* Task/scene node: elements of the per-entity task_node_lists and g_TaskNodePool.
 * Walked and freed by Entity_MarkNodeFree. */
typedef struct FieldActorNode {
    /* 0x00 */ unsigned char pad_00[0x08];
    /* 0x08 */ unsigned short flags;   /* |= 0x10 marks the node free (Entity_MarkNodeFree) */
    /* 0x0A */ unsigned char pad_0A[0x1A];
    /* 0x24 */ struct FieldActorNode *next;
} FieldActorNode;

#define FIELD_ACTOR_POS_X 0x28
#define FIELD_ACTOR_POS_Y 0x2C
#define FIELD_ACTOR_POS_Z 0x30
#define FIELD_ACTOR_BASE_X 0x40
#define FIELD_ACTOR_BASE_Y 0x44
#define FIELD_ACTOR_BASE_Z 0x48
#define FIELD_ACTOR_MODE 0x0E
#define FIELD_ACTOR_ACTION 0x0F
#define FIELD_ACTOR_ACTION_DATA 0x14
#define FIELD_ACTOR_MOTION_X 0x68
#define FIELD_ACTOR_MOTION_Y 0x6C
#define FIELD_ACTOR_MOTION_Z 0x70
#define FIELD_ACTOR_FLAGS 0x98
#define FIELD_ACTOR_DESCRIPTOR_A 0x210
#define FIELD_ACTOR_DESCRIPTOR_B 0x212

#define FIELD_ACTOR_STATE_PROGRESS 0x08
#define FIELD_ACTOR_STATE_AMOUNT 0x0C
#define FIELD_ACTOR_STATE_AMOUNT_MIRROR 0x0E
#define FIELD_ACTOR_STATE_ACTOR_THRESHOLD 0x10
#define FIELD_ACTOR_STATE_SAVED_ACTOR_MODE 0x12
#define FIELD_ACTOR_STATE_COUNT_LIMIT 0x1C
#define FIELD_ACTOR_STATE_DIVISOR_BASIS 0x20
#define FIELD_ACTOR_STATE_ACTOR_THRESHOLD_DELTA 0x24
#define FIELD_ACTOR_STATE_PROGRESS_TARGET 0x28
#define FIELD_ACTOR_STATE_PROGRESS_STEP 0x2C
#define FIELD_ACTOR_STATE_COUNTDOWN 0x30
#define FIELD_ACTOR_STATE_TERMINAL_DELAY 0x34
#define FIELD_ACTOR_STATE_FLAGS 0x4C
#define FIELD_ACTOR_STATE_SUBSTATE 0x6C

/*
 * Do not add canonical externs for g_PlayerEntity/g_ActiveActor here yet. Matching
 * functions currently use several declaration shapes to select absolute vs
 * gp-relative addressing. Prefer local externs until a whole actor cluster is
 * migrated and build-checked.
 */

#endif
