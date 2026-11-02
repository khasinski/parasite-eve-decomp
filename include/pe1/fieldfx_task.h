#ifndef PE1_FIELDFX_TASK_H
#define PE1_FIELDFX_TASK_H

#include "common.h"
#include "pe1/field_billboard.h"
#include "pe1/field_glow_sprite.h"
#include "pe1/field_model_draw.h"
#include "pe1/field_oriented_sprite.h"
#include "pe1/fieldfx_callback_list.h"
#include "pe1/fieldfx_task_pool.h"
#include "pe1/gte_types.h"
#include "pe1/room_floor.h"

extern GteMatrix g_BurstEffectScaleMatrix0, g_BurstEffectScaleMatrix1, g_BurstEffectScaleMatrix2;
extern GteVector g_BurstEffectScaleA, g_BurstEffectScaleB, g_BurstEffectScaleC;
extern char g_BurstEffectScript[];
int BurstEffect_PlantScript(char *object);

typedef FieldFxCallbackListCallback FieldFxTaskCallback;

/* Task dispatch prefix; the owning table also has later lifecycle callbacks. */
typedef struct FieldFxTaskTable {
    int (*callbacks[8])(int mode, void *state, int argument);
    u16 sizes[8];
} FieldFxTaskTable;

/* Task program registered in g_FieldFxTaskPrograms: dispatch table, initializer and script. */
typedef struct FieldFxTaskProgram {
    FieldFxTaskTable table;
    int (*initialize)(int, int, int, int);
    u16 *script;
} FieldFxTaskProgram;
PE1_STATIC_ASSERT(PE1_OFFSETOF(FieldFxTaskProgram, initialize) == 0x30,
                  fieldfx_program_initializer);
PE1_STATIC_ASSERT(PE1_OFFSETOF(FieldFxTaskProgram, script) == 0x34,
                  fieldfx_program_script);
extern char g_FieldFxTaskProgramMissingMsg[]; /* "TS No Thread No.%d\n" */

typedef struct FieldFxTaskContext {
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
    FieldFxTaskSlot slots[8];
    FieldFxTaskTable *table;
    char arena[0x97C];
} FieldFxTaskContext;

PE1_STATIC_ASSERT(sizeof(FieldFxTaskSlot) == 0xC, fieldfx_task_slot_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(FieldFxTaskSlot, start) == 4,
                  fieldfx_task_slot_start);
PE1_STATIC_ASSERT(PE1_OFFSETOF(FieldFxTaskTable, sizes) == 0x20,
                  fieldfx_task_table_sizes);
PE1_STATIC_ASSERT(PE1_OFFSETOF(FieldFxCallbackList, callback) == 8,
                  fieldfx_callback_list_callback);
PE1_STATIC_ASSERT(PE1_OFFSETOF(FieldFxCallbackList, entries) == 0xC,
                  fieldfx_callback_list_entries);
PE1_STATIC_ASSERT(PE1_OFFSETOF(FieldFxCallbackListEntry, state) == 4,
                  fieldfx_callback_list_entry_state);
PE1_STATIC_ASSERT(PE1_OFFSETOF(FieldFxTaskContext, count) == 0xC,
                  fieldfx_task_context_count);
PE1_STATIC_ASSERT(PE1_OFFSETOF(FieldFxTaskContext, used) == 0x10,
                  fieldfx_task_context_used);
PE1_STATIC_ASSERT(PE1_OFFSETOF(FieldFxTaskContext, slots) == 0x20,
                  fieldfx_task_context_slots);
PE1_STATIC_ASSERT(PE1_OFFSETOF(FieldFxTaskContext, table) == 0x80,
                  fieldfx_task_context_table);
PE1_STATIC_ASSERT(PE1_OFFSETOF(FieldFxTaskContext, arena) == 0x84,
                  fieldfx_task_context_arena);
PE1_STATIC_ASSERT(sizeof(FieldFxTaskContext) == 0xA00,
                  fieldfx_task_context_size);

int FieldFx_SpawnTask(int id);

int FieldFx_UpdateCallbackList(void *arg0);

typedef struct FieldFxEmitter {
    int angle;
} FieldFxEmitter;

/* Prefix of the 16-byte payload allocated by this emitter. */
typedef struct FieldFxEmittedPoint {
    s16 position[3];
    s16 state;
    s16 value08;
    s16 angle;
    s16 phase;
} FieldFxEmittedPoint;

/* Task context embedded immediately after the observed owner prefix. */
typedef struct FieldFxTaskOwner {
    FieldFxObjectPrefix prefix;
    FieldFxTaskContext tasks;
} FieldFxTaskOwner;
/* Binds task program `index` (g_FieldFxTaskPrograms) to the owner's tasks. */
int FieldFx_BindTaskProgram(FieldFxTaskOwner *owner, int index);
PE1_STATIC_ASSERT(PE1_OFFSETOF(FieldFxObjectPrefix, asset_type) == 1,
                  fieldfx_owner_asset_type);
PE1_STATIC_ASSERT(PE1_OFFSETOF(FieldFxTaskOwner, tasks) == 0xC,
                  fieldfx_owner_tasks);
PE1_STATIC_ASSERT(PE1_OFFSETOF(FieldFxTaskOwner, tasks.slots) == 0x2C,
                  fieldfx_owner_slots);
PE1_STATIC_ASSERT(PE1_OFFSETOF(FieldFxTaskOwner, tasks.flags) == 0x19,
                  fieldfx_owner_flags);
PE1_STATIC_ASSERT(PE1_OFFSETOF(FieldFxTaskOwner, tasks.table) == 0x8C,
                  fieldfx_owner_table);
int FieldFx_InitTaskContext(FieldFxTaskOwner *owner);
int FieldFx_InitTaskProgram(FieldFxTaskOwner *owner, int skip,
                  int arg2, int arg3, int arg4, int arg5);
int FieldFx_DrawTasks(FieldFxTaskOwner *owner);
extern s16 g_SpinningModelAnchorB[3];
int FieldFx_SpinningModelEmitterB(int mode, FieldFxEmitter *state);

extern u8 *D_800F32D8;
extern u8 *D_800F2D98;
extern s16 g_SpinningModelAnchorA[3];
/* Spinning model flare shared by FieldFx_SpinningModelFlareA and FieldFx_SpinningModelFlareB: the leader
 * (stage 0) follows an anchor point and drops stage 1 copies that shrink. */
typedef struct FieldFxSpinningModel {
    s16 x, y, z;      /* 0x00 */
    s16 stage;        /* 0x06 */
    s16 size;         /* 0x08 */
    s16 angle;        /* 0x0A */
    s16 ticks;        /* 0x0C */
    s16 height;       /* 0x0E */
} FieldFxSpinningModel;
int FieldFx_SpinningModelFlareA(int mode, FieldFxSpinningModel *state);
int FieldFx_SpinningModelFlareB(int mode, FieldFxSpinningModel *state);

/* Twin-model flare (FieldFx_TwinModelEffect): stage 0 grows, stage 1 spins a second
 * white copy, stage 2 holds. */
typedef struct FieldFxTwinModel {
    GteShortVector position;      /* 0x00 */
    int stage;                    /* 0x08 */
    int timer;                    /* 0x0C */
    int angle;                    /* 0x10 */
} FieldFxTwinModel;
extern u8 *D_800F3418;
extern u8 *D_800F2ED8;
extern u8 *g_FieldFxTwinModelB;
extern u8 g_TwinModelColorTrack[];
int FieldFx_TwinModelEffect(int mode, FieldFxTwinModel *state);
int FieldFx_PointEmitter(int mode, FieldFxEmitter *state);

typedef struct FieldFxPointTriple {
    u16 x;
    u16 y;
    u16 z;
} FieldFxPointTriple;

/* Single-point sprite payload: setup stores a position and extent, render
 * copies the extent to all three axes, and update fades scale while growing
 * the extent. This differs from the array payload below: +4 is an extent,
 * and the position begins at +6. */
typedef struct FieldFxPointSprite {
    u8 reserved00[3];
    u8 scale;
    s16 extent;
    FieldFxPointTriple point;
} FieldFxPointSprite;

PE1_STATIC_ASSERT(sizeof(FieldFxPointSprite) == 12,
                  fieldfx_point_sprite_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(FieldFxPointSprite, extent) == 4,
                  fieldfx_point_sprite_extent);
PE1_STATIC_ASSERT(PE1_OFFSETOF(FieldFxPointSprite, point) == 6,
                  fieldfx_point_sprite_position);

typedef struct FieldFxPointData {
    u8 unused_00[3];
    u8 scale;
    s16 count;
    u16 unused_06;
    u16 x[16];
    u16 y[16];
    u16 z[16];
} FieldFxPointData;

/* Scatter initializer writes positions plus three parallel velocity arrays.
 * The point prefix is shared with the point renderer. */
typedef struct FieldFxScatteredParticles {
    FieldFxPointData points;
    s16 velocity_x[16];
    s16 velocity_y[16];
    s16 velocity_z[16];
} FieldFxScatteredParticles;

PE1_STATIC_ASSERT(sizeof(FieldFxPointData) == 0x68,
                  fieldfx_point_data_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(FieldFxScatteredParticles, velocity_x) == 0x68,
                  fieldfx_scattered_velocity_x);
PE1_STATIC_ASSERT(PE1_OFFSETOF(FieldFxScatteredParticles, velocity_y) == 0x88,
                  fieldfx_scattered_velocity_y);
PE1_STATIC_ASSERT(PE1_OFFSETOF(FieldFxScatteredParticles, velocity_z) == 0xA8,
                  fieldfx_scattered_velocity_z);
PE1_STATIC_ASSERT(sizeof(FieldFxScatteredParticles) == 0xC8,
                  fieldfx_scattered_particles_size);

void FieldFx_InitScatteredParticles(void *arg0, void *arg1, FieldFxScatteredParticles *state);

/* Radial emitter: sixteen halfword extents followed by two arrays of
 * eight-byte XYZ records. Setup selects eight or sixteen active points;
 * the script allocation table reserves 0x126 bytes for this payload. */
typedef struct FieldFxRadialPoint {
    FieldFxPointTriple position;
    u16 reserved06;
} FieldFxRadialPoint;

typedef struct FieldFxRadialParticles {
    u8 reserved00;
    u8 scale;
    s8 count;
    u8 reserved03[3];
    u16 extent[16];
    FieldFxRadialPoint points[16];
    GteShortVector velocity[16];
} FieldFxRadialParticles;

PE1_STATIC_ASSERT(sizeof(FieldFxRadialPoint) == 8, fieldfx_radial_point_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(FieldFxRadialParticles, extent) == 6,
                  fieldfx_radial_extent);
PE1_STATIC_ASSERT(PE1_OFFSETOF(FieldFxRadialParticles, points) == 0x26,
                  fieldfx_radial_points);
PE1_STATIC_ASSERT(PE1_OFFSETOF(FieldFxRadialParticles, velocity) == 0xA6,
                  fieldfx_radial_velocity);
PE1_STATIC_ASSERT(sizeof(FieldFxRadialParticles) == 0x126,
                  fieldfx_radial_particles_size);

typedef struct FieldFxBurstWindow {
    u8 unused_00[0x10];
    FieldFxPointTriple point;
    u8 unused_16[0xA];
    FieldFxPointTriple offset;
} FieldFxBurstWindow;

typedef struct FieldFxBurstHeader {
    u8 unused_00[3];
    u8 shade; /* renderer cell brightness, doubled */
    u16 scale;
    u16 duration;
    s16 origin_x;
    s16 origin_y;
    s16 origin_z;
} FieldFxBurstHeader;

typedef union FieldFxBurstData {
    FieldFxBurstHeader header;
    u8 bytes[0x30];
} FieldFxBurstData;

/* Both renderers consume the same sprite storage: the oriented path uses
 * all three scale components, while the billboard path uses X and Y. */
typedef union FieldFxSpriteState {
    FieldOrientedSprite oriented;
    FieldBillboard billboard;
} FieldFxSpriteState;
PE1_STATIC_ASSERT(PE1_OFFSETOF(FieldOrientedSprite, scale) == 0x10,
                  fieldfx_sprite_scale);
PE1_STATIC_ASSERT(PE1_OFFSETOF(FieldOrientedSprite, brightness) == 0x28,
                  fieldfx_sprite_brightness);
PE1_STATIC_ASSERT(PE1_OFFSETOF(FieldBillboard, scaleX) == 0x10,
                  fieldfx_billboard_scale);
PE1_STATIC_ASSERT(PE1_OFFSETOF(FieldBillboard, brightness) == 0x28,
                  fieldfx_billboard_brightness);
extern FieldFxSpriteState g_BurstEffectBState2, g_BurstEffectBState0, g_BurstEffectBState1;

extern FieldFxSpriteState g_PointSpriteGroundRingSprite, g_PointSpriteState3, g_PointSpriteState4, g_PointSpriteStreakSprite;
extern FieldFxSpriteState g_PointSpriteState0, g_PointSpriteState1, g_BurstEffectCState;
extern FieldFxPointTriple g_BurstEffectTargetPosition;

#endif
