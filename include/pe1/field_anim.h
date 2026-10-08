#ifndef PE1_FIELD_ANIM_H
#define PE1_FIELD_ANIM_H

#include "common.h"
#include "pe1/field_anim_callback_list.h"
#include "pe1/gte_types.h"
#include "pe1/room_floor.h"
#include "pe1/field_oriented_sprite.h"
#include "pe1/field_billboard.h"

/* Twelve-byte parameter blocks used by the burst render callbacks.
 * Byte 3 and halfword 6 are not written by setup. */
typedef struct FieldAnimBurstParameters {
    u8 r, g, b, reserved03;
    u8 parameter04, parameter05;
    u16 reserved06;
    s16 parameter08, parameter0A;
} FieldAnimBurstParameters;

PE1_STATIC_ASSERT(sizeof(FieldAnimBurstParameters) == 12,
                  field_anim_burst_parameters_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(FieldAnimBurstParameters, parameter04) == 4,
                  field_anim_burst_parameter04);
PE1_STATIC_ASSERT(PE1_OFFSETOF(FieldAnimBurstParameters, parameter08) == 8,
                  field_anim_burst_parameter08);
PE1_STATIC_ASSERT(PE1_OFFSETOF(FieldAnimBurstParameters, parameter0A) == 10,
                  field_anim_burst_parameter0a);

extern GteMatrix D_800F3478, D_800F33C0, D_800F32B0;
extern GteVector D_800C220C, D_800C221C, D_800C222C;
extern FieldAnimBurstParameters D_800E2298, D_800E2250, D_800E27E0, D_800F3460;
extern char D_800E0EB8[];
int func_800CCBA8(char *object);

typedef FieldAnimCallbackListCallback FieldAnimTaskCallback;

typedef struct FieldAnimTaskSlot {
    u16 id;              /* 0xFFFF marks a free slot. */
    u16 age;
    char *start;
    char *end;
} FieldAnimTaskSlot;

/* Task dispatch prefix; the owning table also has later lifecycle callbacks. */
typedef struct FieldAnimTaskTable {
    int (*callbacks[8])(int mode, void *state, int argument);
    u16 sizes[8];
} FieldAnimTaskTable;

/* Task program registered in D_800E1044: dispatch table, initializer and script. */
typedef struct FieldAnimTaskProgram {
    FieldAnimTaskTable table;
    int (*initialize)(int, int, int, int);
    u16 *script;
} FieldAnimTaskProgram;
PE1_STATIC_ASSERT(PE1_OFFSETOF(FieldAnimTaskProgram, initialize) == 0x30,
                  field_anim_program_initializer);
PE1_STATIC_ASSERT(PE1_OFFSETOF(FieldAnimTaskProgram, script) == 0x34,
                  field_anim_program_script);
extern char D_800C2244[]; /* "TS No Thread No.%d\n" */

typedef struct FieldAnimTaskContext {
    /* Opcode 3 loads its 16-bit operand as the new script address. */
    union {
        u16 *script;
        int address;
    } pc;
    char *cursor;
    int argument;
    u8 count;
    u8 flags;
    u16 delay;
    u16 used;
    s16 variables[7];
    FieldAnimTaskSlot slots[8];
    FieldAnimTaskTable *table;
    char arena[0x97C];
} FieldAnimTaskContext;

PE1_STATIC_ASSERT(sizeof(FieldAnimTaskSlot) == 0xC, field_anim_task_slot_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(FieldAnimTaskSlot, start) == 4,
                  field_anim_task_slot_start);
PE1_STATIC_ASSERT(PE1_OFFSETOF(FieldAnimTaskTable, sizes) == 0x20,
                  field_anim_task_table_sizes);
PE1_STATIC_ASSERT(PE1_OFFSETOF(FieldAnimCallbackList, callback) == 8,
                  field_anim_callback_list_callback);
PE1_STATIC_ASSERT(PE1_OFFSETOF(FieldAnimCallbackList, entries) == 0xC,
                  field_anim_callback_list_entries);
PE1_STATIC_ASSERT(PE1_OFFSETOF(FieldAnimCallbackListEntry, state) == 4,
                  field_anim_callback_list_entry_state);
PE1_STATIC_ASSERT(PE1_OFFSETOF(FieldAnimTaskContext, count) == 0xC,
                  field_anim_task_context_count);
PE1_STATIC_ASSERT(PE1_OFFSETOF(FieldAnimTaskContext, used) == 0x10,
                  field_anim_task_context_used);
PE1_STATIC_ASSERT(PE1_OFFSETOF(FieldAnimTaskContext, slots) == 0x20,
                  field_anim_task_context_slots);
PE1_STATIC_ASSERT(PE1_OFFSETOF(FieldAnimTaskContext, table) == 0x80,
                  field_anim_task_context_table);
PE1_STATIC_ASSERT(PE1_OFFSETOF(FieldAnimTaskContext, arena) == 0x84,
                  field_anim_task_context_arena);
PE1_STATIC_ASSERT(sizeof(FieldAnimTaskContext) == 0xA00,
                  field_anim_task_context_size);

extern FieldAnimTaskContext *D_800E2368;
int func_800D401C(int id);

extern FieldAnimTaskSlot *D_800F33E0;
int func_800CE560(char *out, int stride, int count, FieldAnimTaskCallback callback);
void *func_800CE610(char *list);
int func_800CE5AC(void *arg0, int arg1, int arg2, int arg3, void *arg4);
int func_800CE688(void *arg0);
int func_800CE78C(char *arg0);

typedef struct FieldAnimEmitter {
    int angle;
} FieldAnimEmitter;

/* Prefix of the 16-byte payload allocated by this emitter. */
typedef struct FieldAnimEmittedPoint {
    s16 position[3];
    s16 state;
    s16 value08;
    s16 angle;
    s16 phase;
} FieldAnimEmittedPoint;

/* Observed prefix of the active effect owner; actor is at +0x08. */
typedef struct FieldAnimObjectPrefix {
    u8 reserved00;
    u8 asset_type;
    u8 reserved02[6];
    struct FieldActor *actor;
} FieldAnimObjectPrefix;
PE1_STATIC_ASSERT(PE1_OFFSETOF(FieldAnimObjectPrefix, actor) == 8,
                  field_anim_object_actor_offset);
/* Task context embedded immediately after the observed owner prefix. */
typedef struct FieldAnimTaskOwner {
    FieldAnimObjectPrefix prefix;
    FieldAnimTaskContext tasks;
} FieldAnimTaskOwner;
PE1_STATIC_ASSERT(PE1_OFFSETOF(FieldAnimObjectPrefix, asset_type) == 1,
                  field_anim_owner_asset_type);
PE1_STATIC_ASSERT(PE1_OFFSETOF(FieldAnimTaskOwner, tasks) == 0xC,
                  field_anim_owner_tasks);
PE1_STATIC_ASSERT(PE1_OFFSETOF(FieldAnimTaskOwner, tasks.slots) == 0x2C,
                  field_anim_owner_slots);
PE1_STATIC_ASSERT(PE1_OFFSETOF(FieldAnimTaskOwner, tasks.flags) == 0x19,
                  field_anim_owner_flags);
PE1_STATIC_ASSERT(PE1_OFFSETOF(FieldAnimTaskOwner, tasks.table) == 0x8C,
                  field_anim_owner_table);
int func_800D4620(FieldAnimTaskOwner *owner);
int func_800D4698(FieldAnimTaskOwner *owner, int skip,
                  int arg2, int arg3, int arg4, int arg5);
int func_800D4704(FieldAnimTaskOwner *owner);
extern FieldAnimObjectPrefix *D_800F32D0;
extern s16 D_800E2214[3];
int func_800DA5D4(int mode, FieldAnimEmitter *state);

extern u8 *D_800F32D8;
extern s16 D_800E220C[];
void func_800C6D5C(u8 *data, u8 x_offset, u8 y_offset);
/* Spinning model flare shared by func_800D9A8C and func_800DA1FC: the leader
 * (stage 0) follows an anchor point and drops stage 1 copies that shrink. */
typedef struct FieldAnimSpinningModel {
    s16 x, y, z;      /* 0x00 */
    s16 stage;        /* 0x06 */
    s16 size;         /* 0x08 */
    s16 angle;        /* 0x0A */
    s16 ticks;        /* 0x0C */
    s16 height;       /* 0x0E */
} FieldAnimSpinningModel;
int func_800D9A8C(int mode, FieldAnimSpinningModel *state);
int func_800DA1FC(int mode, FieldAnimSpinningModel *state);

/* Twin-model flare (func_800DC058): stage 0 grows, stage 1 spins a second
 * white copy, stage 2 holds. */
typedef struct FieldAnimTwinModel {
    GteShortVector position;      /* 0x00 */
    int stage;                    /* 0x08 */
    int timer;                    /* 0x0C */
    int angle;                    /* 0x10 */
} FieldAnimTwinModel;
extern u8 *D_800F3418;
extern u8 *D_800F342C;
extern u8 D_800E1E24[];
int func_800DC058(int mode, FieldAnimTwinModel *state);
int FieldEng_PointEmitter(int mode, FieldAnimEmitter *state);

typedef struct FieldAnimPointTriple {
    u16 x;
    u16 y;
    u16 z;
} FieldAnimPointTriple;

/* Single-point sprite payload: setup stores a position and extent, render
 * copies the extent to all three axes, and update fades scale while growing
 * the extent. This differs from the array payload below: +4 is an extent,
 * and the position begins at +6. */
typedef struct FieldAnimPointSprite {
    u8 reserved00[3];
    u8 scale;
    s16 extent;
    FieldAnimPointTriple point;
} FieldAnimPointSprite;

PE1_STATIC_ASSERT(sizeof(FieldAnimPointSprite) == 12,
                  field_anim_point_sprite_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(FieldAnimPointSprite, extent) == 4,
                  field_anim_point_sprite_extent);
PE1_STATIC_ASSERT(PE1_OFFSETOF(FieldAnimPointSprite, point) == 6,
                  field_anim_point_sprite_position);

typedef struct FieldAnimPointData {
    u8 unused_00[3];
    u8 scale;
    s16 count;
    u16 unused_06;
    u16 x[16];
    u16 y[16];
    u16 z[16];
} FieldAnimPointData;

/* Scatter initializer writes positions plus three parallel velocity arrays.
 * The point prefix is shared with the point renderer. */
typedef struct FieldAnimScatteredParticles {
    FieldAnimPointData points;
    s16 velocity_x[16];
    s16 velocity_y[16];
    s16 velocity_z[16];
} FieldAnimScatteredParticles;

PE1_STATIC_ASSERT(sizeof(FieldAnimPointData) == 0x68,
                  field_anim_point_data_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(FieldAnimScatteredParticles, velocity_x) == 0x68,
                  field_anim_scattered_velocity_x);
PE1_STATIC_ASSERT(PE1_OFFSETOF(FieldAnimScatteredParticles, velocity_y) == 0x88,
                  field_anim_scattered_velocity_y);
PE1_STATIC_ASSERT(PE1_OFFSETOF(FieldAnimScatteredParticles, velocity_z) == 0xA8,
                  field_anim_scattered_velocity_z);
PE1_STATIC_ASSERT(sizeof(FieldAnimScatteredParticles) == 0xC8,
                  field_anim_scattered_particles_size);

void func_800CC2C4(void *arg0, void *arg1, FieldAnimScatteredParticles *state);

/* Radial emitter: sixteen halfword extents followed by two arrays of
 * eight-byte XYZ records. Setup selects eight or sixteen active points;
 * the script allocation table reserves 0x126 bytes for this payload. */
typedef struct FieldAnimRadialPoint {
    FieldAnimPointTriple position;
    u16 reserved06;
} FieldAnimRadialPoint;

typedef struct FieldAnimRadialParticles {
    u8 reserved00;
    u8 scale;
    s8 count;
    u8 reserved03[3];
    u16 extent[16];
    FieldAnimRadialPoint points[16];
    GteShortVector velocity[16];
} FieldAnimRadialParticles;

PE1_STATIC_ASSERT(sizeof(FieldAnimRadialPoint) == 8, field_anim_radial_point_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(FieldAnimRadialParticles, extent) == 6,
                  field_anim_radial_extent);
PE1_STATIC_ASSERT(PE1_OFFSETOF(FieldAnimRadialParticles, points) == 0x26,
                  field_anim_radial_points);
PE1_STATIC_ASSERT(PE1_OFFSETOF(FieldAnimRadialParticles, velocity) == 0xA6,
                  field_anim_radial_velocity);
PE1_STATIC_ASSERT(sizeof(FieldAnimRadialParticles) == 0x126,
                  field_anim_radial_particles_size);

typedef struct FieldAnimPointState {
    FieldAnimPointTriple point;
    u8 unused_06[0x22];
    short scale;
} FieldAnimPointState;

typedef struct FieldAnimInterleavedState {
    FieldAnimPointTriple point;
    u8 unused_06[0xA];
    int extent_x;
    int extent_y;
    int extent_z;
    u8 unused_1C[0xC];
    short scale;
} FieldAnimInterleavedState;

typedef struct FieldAnimBurstWindow {
    u8 unused_00[0x10];
    FieldAnimPointTriple point;
    u8 unused_16[0xA];
    FieldAnimPointTriple offset;
} FieldAnimBurstWindow;

typedef struct FieldAnimBurstHeader {
    u8 unused_00[3];
    u8 mode;
    u8 scale[2];
    u8 duration[2];
    u8 origin_x[2];
    u8 origin_y[2];
    u8 origin_z[2];
} FieldAnimBurstHeader;

typedef union FieldAnimBurstData {
    FieldAnimBurstHeader header;
    u8 bytes[0x30];
} FieldAnimBurstData;

extern FieldAnimInterleavedState D_800E2260;
/* Both renderers consume the same sprite storage: the oriented path uses
 * all three scale components, while the billboard path uses X and Y. */
typedef union FieldAnimSpriteState {
    FieldOrientedSprite oriented;
    FieldBillboard billboard;
} FieldAnimSpriteState;
PE1_STATIC_ASSERT(PE1_OFFSETOF(FieldOrientedSprite, scale) == 0x10,
                  field_anim_sprite_scale);
PE1_STATIC_ASSERT(PE1_OFFSETOF(FieldOrientedSprite, brightness) == 0x28,
                  field_anim_sprite_brightness);
PE1_STATIC_ASSERT(PE1_OFFSETOF(FieldBillboard, scaleX) == 0x10,
                  field_anim_billboard_scale);
PE1_STATIC_ASSERT(PE1_OFFSETOF(FieldBillboard, brightness) == 0x28,
                  field_anim_billboard_brightness);
extern FieldAnimSpriteState D_800F32E0, D_800F3338, D_800F3380, D_800F3430;
extern FieldAnimPointState D_800E2818;
extern FieldAnimPointTriple D_800E27F8;

#endif
