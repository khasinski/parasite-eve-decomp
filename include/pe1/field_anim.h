#ifndef PE1_FIELD_ANIM_H
#define PE1_FIELD_ANIM_H

#include "common.h"
#include "pe1/gte_types.h"

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

typedef int (*FieldAnimTaskCallback)(int mode, void *state);

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

typedef struct FieldAnimTaskContext {
    u16 *script;
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
int func_800CE688(char *arg0);
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
    u32 unknown00[2];
    struct FieldActor *actor;
} FieldAnimObjectPrefix;
PE1_STATIC_ASSERT(PE1_OFFSETOF(FieldAnimObjectPrefix, actor) == 8,
                  field_anim_object_actor_offset);
extern FieldAnimObjectPrefix *D_800F32D0;
extern s16 D_800E2214[3];
extern s16 D_800942EC;
int func_800DA1FC(int mode, void *state);
int func_800DA5D4(int mode, FieldAnimEmitter *state);

extern u8 *D_800F32D8;
extern s16 D_800E220C[];
void func_800C6D5C(u8 *data, u8 x_offset, u8 y_offset);
int func_800D9A8C(int mode, void *state);
int FieldEng_PointEmitter(int mode, FieldAnimEmitter *state);

typedef struct FieldAnimPointTriple {
    u16 x;
    u16 y;
    u16 z;
} FieldAnimPointTriple;

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

typedef struct FieldAnimInterleavedWindow {
    u8 unused_00[0x26];
    FieldAnimPointTriple point;
} FieldAnimInterleavedWindow;

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
extern FieldAnimPointState D_800E2818;
extern FieldAnimPointTriple D_800E27F8;

#endif
