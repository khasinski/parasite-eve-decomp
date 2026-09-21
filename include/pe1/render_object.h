#ifndef PE1_RENDER_OBJECT_H
#define PE1_RENDER_OBJECT_H

#include "common.h"
#include "pe1/gte_types.h"

/* Packed geometry at 0x800C7AE0: two groups of 12-byte records and two
 * groups of 16-byte records precede the 8-byte vertex array. The record
 * kinds and the remaining header words are not identified yet. */
typedef union RenderPackedGeometry {
    struct {
        u16 record12_counts[2];
        u16 record16_counts[2];
        u16 reserved[4];
    } counts;
    /* Variable-size asset view; the header is followed by records/vertices. */
    u8 bytes[1];
} RenderPackedGeometry;

PE1_STATIC_ASSERT(sizeof(RenderPackedGeometry) == 16,
                  render_packed_geometry_header_size);

void FieldEng_TransformPackedVertex(RenderPackedGeometry *geometry,
                                   GteMatrix *matrix, u16 index,
                                   GteShortVector *out);
void FieldEng_CalculateLookAngles(GteShortVector *from, GteShortVector *to,
                                 GteShortVector *out);

/* History entries use halfword XYZ writes when filled, but aligned word
 * copies (including padding) when shifted or replaced. */
typedef union RenderHistoryPoint {
    GteShortVector vector;
    u32 words[2];
} RenderHistoryPoint;

PE1_STATIC_ASSERT(sizeof(RenderHistoryPoint) == 8, render_history_point_size);

void FieldEng_UpdatePointHistory(RenderHistoryPoint *history, s16 count,
                                 RenderHistoryPoint *value, int reset);

typedef struct RenderCosineEffect {
    s16 x;
    s16 amplitude;
    s16 y;
    s16 duration;
} RenderCosineEffect;

PE1_STATIC_ASSERT(sizeof(RenderCosineEffect) == 8, render_cosine_effect_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderCosineEffect, duration) == 6,
                  render_cosine_effect_duration);
int func_800D5898(int mode, RenderCosineEffect *state);

/* Observed halfword parameter block at 0x800F3368. Individual legacy
 * declarations still exist in other units; this declaration owns no storage. */
typedef struct RenderEffectParameters {
    u16 parameter00;
    u16 parameter02;
    u16 palette;
    u16 parameter06;
    u16 tpage;
    u16 parameter0A;
    s16 depth;
    u16 extent_x;
    u16 extent_y;
} RenderEffectParameters;

PE1_STATIC_ASSERT(sizeof(RenderEffectParameters) == 0x12,
                  render_effect_parameters_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderEffectParameters, palette) == 4,
                  render_effect_parameters_palette);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderEffectParameters, depth) == 0xC,
                  render_effect_parameters_depth);
extern RenderEffectParameters D_800F3368;

typedef struct RenderArcingEmitter {
    GteShortVector position;
    int phase;
    int radius;
} RenderArcingEmitter;

PE1_STATIC_ASSERT(sizeof(RenderArcingEmitter) == 0x10,
                  render_arcing_emitter_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderArcingEmitter, radius) == 0xC,
                  render_arcing_emitter_radius);
int func_800D7B70(int mode, RenderArcingEmitter *state);
extern u8 D_800E18F0[];
extern u16 D_800E11E6;
extern u16 D_800E11F6;
extern u16 D_800E2850[];
void func_800CEDA8(int index);

typedef struct RenderOrbitingEffect {
    s16 stage;
    s16 angle;
    s16 timer;
    s16 x, y, z;
    s16 radius;
    s16 reserved0E;
} RenderOrbitingEffect;

PE1_STATIC_ASSERT(sizeof(RenderOrbitingEffect) == 0x10,
                  render_orbiting_effect_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderOrbitingEffect, x) == 6,
                  render_orbiting_effect_position);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderOrbitingEffect, radius) == 0xC,
                  render_orbiting_effect_radius);
int func_800D7FBC(int mode, RenderOrbitingEffect *state);
typedef struct RenderOrbitingEmitter {
    int phase;
    char *particles;
} RenderOrbitingEmitter;

PE1_STATIC_ASSERT(sizeof(RenderOrbitingEmitter) == 8,
                  render_orbiting_emitter_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderOrbitingEmitter, particles) == 4,
                  render_orbiting_emitter_particles);
int func_800D868C(int mode, RenderOrbitingEmitter *state);
extern GteShortVector D_800E21EC;
extern char *D_800E21F4;

/* The neighboring emitter allocates 12-byte payloads for this callback. */
typedef struct RenderVerticalEffect {
    s16 stage;
    s16 angle;
    s16 timer;
    s16 x, y, z;
} RenderVerticalEffect;

PE1_STATIC_ASSERT(sizeof(RenderVerticalEffect) == 0xC,
                  render_vertical_effect_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderVerticalEffect, timer) == 4,
                  render_vertical_effect_timer);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderVerticalEffect, x) == 6,
                  render_vertical_effect_position);
int func_800D8978(int mode, RenderVerticalEffect *state);
int func_800D8B6C(int mode, void *state);
extern GteShortVector D_800E21F8;
extern u8 D_800E1A14[];

/* The controller at 0x800D927C allocates 16-byte effects and 8-byte particles. */
typedef struct RenderHelicalEffect {
    s16 stage;
    s16 angle;
    s16 timer;
    s16 x, y, z;
    s16 radius;
    s16 direction;
} RenderHelicalEffect;

PE1_STATIC_ASSERT(sizeof(RenderHelicalEffect) == 0x10,
                  render_helical_effect_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderHelicalEffect, x) == 6,
                  render_helical_effect_position);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderHelicalEffect, direction) == 0xE,
                  render_helical_effect_direction);
int func_800D8D14(int mode, GteShortVector *state);
int func_800D8E74(int mode, RenderHelicalEffect *state);
int func_800D927C(int mode, RenderOrbitingEmitter *state);
extern u8 D_800E1AA0[];
extern GteShortVector D_800E2200;
extern char *D_800E2208;

typedef struct RenderDampedSpark {
    s16 x, y, z;
    s16 vx, vy, vz;
} RenderDampedSpark;

typedef struct RenderSparkEmitter {
    GteShortVector position;
    int phase;
} RenderSparkEmitter;

PE1_STATIC_ASSERT(sizeof(RenderDampedSpark) == 0xC, render_damped_spark_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderDampedSpark, vx) == 6,
                  render_damped_spark_velocity);
PE1_STATIC_ASSERT(sizeof(RenderSparkEmitter) == 0xC, render_spark_emitter_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderSparkEmitter, phase) == 8,
                  render_spark_emitter_phase);
int func_800D9554(int mode, RenderDampedSpark *state);
int func_800D96F4(int mode, RenderSparkEmitter *state);
void func_800D1DEC(void *position, void *color, int scale, int flags);
extern u8 D_800E1AC8[];

typedef struct RenderArcingEffect {
    s16 x, y, z;
    s16 velocity_y;
} RenderArcingEffect;

PE1_STATIC_ASSERT(sizeof(RenderArcingEffect) == 8, render_arcing_effect_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderArcingEffect, velocity_y) == 6,
                  render_arcing_effect_velocity);
int func_800D7A1C(int mode, RenderArcingEffect *state);
int func_800D9E5C(int mode, RenderArcingEffect *state);
int func_800DA780(int mode, RenderArcingEffect *state);
int func_800DA934(int mode, GteShortVector *state);
int func_800D9FD4(int mode, GteShortVector *state);
extern u8 D_800E18C0[];

typedef struct RenderSineEffect {
    GteShortVector position;
    s16 angle;
    s16 scale;
    s16 amplitude;
    s16 velocity_y;
} RenderSineEffect;

PE1_STATIC_ASSERT(sizeof(RenderSineEffect) == 0x10, render_sine_effect_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderSineEffect, angle) == 8,
                  render_sine_effect_angle);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderSineEffect, amplitude) == 0xC,
                  render_sine_effect_amplitude);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderSineEffect, velocity_y) == 0xE,
                  render_sine_effect_velocity);

typedef struct RenderSineEmitter {
    GteShortVector position;
    int phase;
} RenderSineEmitter;

PE1_STATIC_ASSERT(sizeof(RenderSineEmitter) == 0xC, render_sine_emitter_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderSineEmitter, phase) == 8,
                  render_sine_emitter_phase);
int func_800DB0D0(int mode, RenderSineEmitter *state);

extern u8 D_800E1C2C[];
extern GteShortVector D_800E221C;
int func_800DAF8C(int mode, RenderSineEffect *state);
void func_800D0E88(void *data, GteShortVector *position, int scale, int angle,
                   void *color, int arg5, int arg6, int intensity, int mode);

extern int D_800E27EC;
extern u8 D_800E1C04[];
extern int D_800E1D60;
extern u8 D_800E1D64[], D_800E1D84[];
int func_800DBA9C(int mode, RenderSparkEmitter *state);
extern u8 D_800E1DA4[];
extern u8 D_800E1E64[];
int func_800DC5BC(int mode, GteShortVector *state);
int func_800DC750(int mode, GteShortVector *state);
int func_800DBCD8(int mode, GteShortVector *state);
int func_800DBE6C(int mode, GteShortVector *state);
/* Initializes cached key times, clamps time at the end and blends colors.
 * The track requires at least one positive-duration key and a zero-duration
 * terminator carrying the final RGB value. Call with nonnegative time. */
void func_800CF3AC(void *track, void *color, int time);
void func_800D27FC(int x, int y, void *color, int scale, int mode);
int FieldEng_CosineEffect(int mode, RenderCosineEffect *effect);

extern s16 D_800F3374;
extern u8 D_800E1494[];
void func_800CE870(char *object, int mode, s16 *position);
void func_800D0728(GteShortVector *position, int arg1, int arg2, int arg3,
                   GteRotation *rotation, int scale_x, int scale_y,
                   void *data, void *color, int intensity, int mode);
int FieldEng_RotatingEffect(int mode);

typedef struct RenderColor {
    u8 r, g, b, code;
} RenderColor;

extern RenderColor D_800C22C0;

typedef struct RenderDiamondEmitter {
    GteShortVector position;
    int x, y, angle, size, radius;
} RenderDiamondEmitter;
extern GteShortVector D_800E2224;
extern u8 D_800E1CC8[];
typedef struct RenderDiamondParticle {
    s16 x, y, angle, size, reserved, color_time;
} RenderDiamondParticle;
PE1_STATIC_ASSERT(sizeof(RenderDiamondEmitter) == 28, render_diamond_emitter_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderDiamondEmitter, radius) == 24,
                  render_diamond_emitter_radius);
PE1_STATIC_ASSERT(sizeof(RenderDiamondParticle) == 12, render_diamond_particle_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderDiamondParticle, color_time) == 10,
                  render_diamond_particle_color_time);
int func_800DB5F4(int mode, RenderDiamondParticle *state);
int func_800DB6BC(int mode, RenderDiamondEmitter *state);

/* The emitter at 0x800DDD70 allocates this 16-byte callback state. */
typedef struct RenderSettlingSprite {
    GteShortVector position;
    s16 stage, timer, phase, reserved;
} RenderSettlingSprite;
PE1_STATIC_ASSERT(sizeof(RenderSettlingSprite) == 16, render_settling_sprite_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderSettlingSprite, stage) == 8,
                  render_settling_sprite_stage);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderSettlingSprite, phase) == 12,
                  render_settling_sprite_phase);
extern GteShortVector D_800E223C;
extern u8 D_800E20AC[];
int func_800DD9E4(int mode, RenderSettlingSprite *state);
int func_800DDD70(int mode, GteShortVector *state);

/* 0x800DD76C allocates 16-byte particles for 0x800DD380. */
typedef struct RenderConvergingSprite {
    GteShortVector position;
    s16 stage, timer, bursts, phase;
} RenderConvergingSprite;
typedef struct RenderConvergingEmitter {
    GteShortVector position, target;
} RenderConvergingEmitter;
PE1_STATIC_ASSERT(sizeof(RenderConvergingSprite) == 16, render_converging_sprite_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderConvergingSprite, stage) == 8,
                  render_converging_sprite_stage);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderConvergingSprite, phase) == 14,
                  render_converging_sprite_phase);
PE1_STATIC_ASSERT(sizeof(RenderConvergingEmitter) == 16, render_converging_emitter_size);
extern GteShortVector D_800E2234;
extern u8 D_800E1FEC[];
int func_800DD380(int mode, RenderConvergingSprite *state);
int func_800DD76C(int mode, RenderConvergingEmitter *state);

/* Callback 0x800DCCCC and its emitter 0x800DCE94 use a 16-byte payload.
 * The rotation prefix is passed directly to RotMatrixYXZ by 0x800D2370. */
typedef struct RenderTiltingSprite {
    GteRotation rotation;
    s16 height, width;
    s16 initial_tilt, velocity_y;
} RenderTiltingSprite;

PE1_STATIC_ASSERT(sizeof(RenderTiltingSprite) == 16, render_tilting_sprite_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderTiltingSprite, height) == 8,
                  render_tilting_sprite_height);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderTiltingSprite, initial_tilt) == 12,
                  render_tilting_sprite_initial_tilt);
extern u8 D_800E1F18[];
extern GteShortVector D_800E222C;
int func_800DCCCC(int mode, RenderTiltingSprite *state);
int func_800DCE94(int mode, RenderSparkEmitter *state);
void func_800D2370(GteShortVector *position, GteRotation *rotation,
                   int width, int height, int u, int v, int texture_width,
                   int texture_height, int clut, RenderColor *color0,
                   RenderColor *color1, int intensity, int mode);

extern RenderColor D_800C22DC;
extern RenderColor D_800C22E0;
extern RenderColor D_800C22E4;
extern RenderColor D_800C22E8;
extern RenderColor D_800C22EC;
void func_800D004C(GteShortVector *position, int width, int height, int segments,
                   GteRotation *rotation, int scale_x, int scale_y,
                   RenderColor *color0, RenderColor *color1, int intensity, int mode);
int func_800DACA4(int mode, RenderSparkEmitter *state);

PE1_STATIC_ASSERT(sizeof(RenderColor) == 4, render_color_size);

/* In a color-track key, color.code stores the encoded segment duration. */
typedef struct RenderColorKeyTiming {
    u16 length;
    u16 start;
} RenderColorKeyTiming;

typedef struct RenderColorKey {
    RenderColor color;
    RenderColorKeyTiming timing;
} RenderColorKey;

typedef struct RenderColorTrack {
    int duration; /* Zero means the timing cache has not been initialized. */
    int count;
    RenderColorKey keys[0];
} RenderColorTrack;

PE1_STATIC_ASSERT(sizeof(RenderColorKey) == 8, render_color_key_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderColorKey, timing) == 4,
                  render_color_key_timing_offset);
PE1_STATIC_ASSERT(sizeof(RenderColorTrack) == 8, render_color_track_header_size);

void LoadAverageCol(void *first, void *second, int first_scale,
                    int second_scale, void *output);


extern u8 D_800E1988[];
extern u8 D_800E1EE8[];
extern u16 D_800F336C;
extern u16 D_800E1204[];
extern int D_800F3428;
void func_800CEE20(GteShortVector *position, GteRotation *rotation,
                   int scale_x, int scale_y, int texture, int clut,
                   int page, int intensity, RenderColor *color);
int FieldEng_DriftingEffect(int mode, GteShortVector *position);
int func_800DCA80(int mode, GteShortVector *position);
extern u8 D_800E1FA4[];
extern u8 D_800E1FCC[];
int func_800DD19C(int mode, RenderSparkEmitter *state);
int func_800D7E78(int mode, GteShortVector *state);

void FieldEng_RotateVector(const GteMatrixWords *matrix,
                           const GteShortVector *input, GteShortVector *output);

typedef struct RenderMatrixSlot {
    s32 *value;
    u8 reserved[8];
} RenderMatrixSlot;

extern RenderMatrixSlot D_800BCFA4;
void FieldEng_TransformTranslation(const GteShortVector *input,
                                  GteMatrixWords *output);

struct RoomFxTransformOwner;
void FieldEng_TransformMatrixPoint(struct RoomFxTransformOwner *owner, int index,
                                  const GteShortVector *input,
                                  GteShortVector *output);

/* Shared runtime object layout used by the morph and draw paths. */
typedef struct RenderVec3s {
    signed short x;
    signed short y;
    signed short z;
    signed short pad;
} RenderVec3s;

typedef struct RenderObjectPart {
    unsigned short vertex_start;
    unsigned short vertex_count;
    unsigned char visible;
    unsigned char pad_05[7];
} RenderObjectPart;

typedef struct RenderObjectHeader {
    unsigned char pad_00[2];
    unsigned char part_count;
    unsigned char animation_entry_count;
    unsigned char pad_04[4];
    unsigned short packet34_count;
    unsigned short packet28_count;
    unsigned short packet24_count;
    unsigned short packet1c_count;
    unsigned char pad_10[0x0A];
    unsigned short visible_part_count;
} RenderObjectHeader;

typedef struct RenderPrimitiveDescriptor {
    /* 0x00 */ unsigned char reserved00[4];
    /* 0x04 */ u16 lookup_indices[4];
} RenderPrimitiveDescriptor;

/* Shared prefix through the page/depth selector used by the 0x34- and
 * 0x28-byte double-buffered packet classes. */
typedef struct RenderPacketState {
    /* 0x00 */ unsigned char reserved00[0x1A];
    /* 0x1A */ u16 page_bits;
} RenderPacketState;

typedef union RenderPacketValue {
    u32 value;
    struct {
        u8 byte0;
        u8 byte1;
        u8 byte2;
        u8 command;
    } bytes;
} RenderPacketValue;

typedef struct RenderPacket34 {
    /* 0x00 */ u32 tag;
    /* 0x04 */ RenderPacketValue values0;
    /* 0x08 */ u8 reserved08[8];
    /* 0x10 */ u32 value1;
    /* 0x14 */ u8 reserved14[6];
    /* 0x1A */ u16 page_bits;
    /* 0x1C */ u32 value2;
    /* 0x20 */ u8 reserved20[8];
    /* 0x28 */ u32 value3;
    /* 0x2C */ u8 reserved2c[8];
} RenderPacket34;

typedef struct RenderPacket28 {
    /* 0x00 */ u32 tag;
    /* 0x04 */ RenderPacketValue values0;
    /* 0x08 */ u8 reserved08[8];
    /* 0x10 */ u32 value1;
    /* 0x14 */ u8 reserved14[6];
    /* 0x1A */ u16 page_bits;
    /* 0x1C */ u32 value2;
    /* 0x20 */ u8 reserved20[8];
} RenderPacket28;

typedef struct RenderPacket24 {
    /* 0x00 */ u32 tag;
    /* 0x04 */ RenderPacketValue values0;
    /* 0x08 */ u8 reserved08[4];
    /* 0x0C */ u32 value1;
    /* 0x10 */ u8 reserved10[4];
    /* 0x14 */ u32 value2;
    /* 0x18 */ u8 reserved18[4];
    /* 0x1C */ u32 value3;
    /* 0x20 */ u8 reserved20[4];
} RenderPacket24;

typedef struct RenderPacket1C {
    /* 0x00 */ u32 tag;
    /* 0x04 */ RenderPacketValue values0;
    /* 0x08 */ u8 reserved08[4];
    /* 0x0C */ u32 value1;
    /* 0x10 */ u8 reserved10[4];
    /* 0x14 */ u32 value2;
    /* 0x18 */ u8 reserved18[4];
} RenderPacket1C;

typedef struct RenderAnimationLookupEntry {
    /* 0x00 */ s16 value0;
    /* 0x02 */ s16 value1;
    /* 0x04 */ s16 value2;
    /* 0x06 */ s16 animation_id;
    /* 0x08 */ unsigned char reserved08[4];
} RenderAnimationLookupEntry;

typedef struct RenderAnimationDataHeader {
    /* 0x00 */ u8 encoding_flags;
    /* 0x01 */ u8 last_bone_index;
    /* 0x02 */ u8 packing_flags;
    /* 0x03 */ u8 reserved03;
    /* 0x04 */ u16 object_value74;
    /* 0x06 */ u16 object_value76;
    /* 0x08 */ u16 object_value78;
    /* 0x0A */ u16 object_value7c;
} RenderAnimationDataHeader;

/* A nonzero marker selects the inline constant; zero selects frame-indexed
 * samples beginning at the same value byte/halfword. */
typedef struct RenderAnimByteChannel {
    /* 0x00 */ u8 constant_marker;
    /* 0x01 */ u8 value_or_samples[1];
    /* 0x02 */ u8 reserved02[2];
} RenderAnimByteChannel;

typedef struct RenderAnimShortChannel {
    /* 0x00 */ s16 constant_marker;
    /* 0x02 */ union {
        s16 signed_values[1];
        u16 unsigned_values[1];
    } samples;
} RenderAnimShortChannel;

typedef struct RenderRotationOverride {
    /* 0x00 */ u16 x;
    /* 0x02 */ u16 y;
    /* 0x04 */ u16 z;
    /* 0x06 */ u8 matrix_index;
    /* 0x07 */ u8 flags;
} RenderRotationOverride;

/* PSX GTE matrix ABI: nine packed rotation coefficients, alignment, translation. */
typedef struct RenderMatrix {
    /* 0x00 */ s16 rotation[3][3];
    /* 0x12 */ s16 reserved12;
    /* 0x14 */ s32 translation[3];
} RenderMatrix;

typedef struct RenderObjectEntity {
    /* 0x00 */ RenderObjectHeader *header;
    /* 0x04 */ RenderObjectPart *parts;
    /* 0x08 */ RenderVec3s *vertices;
    /* 0x0C */ unsigned int *vertex_colours;
    /* 0x10 */ RenderPrimitiveDescriptor *primitive_descriptors;
    /* 0x14 */ void *model_section14;
    /* 0x18 */ RenderVec3s *bounds_vertices;
    /* 0x1C */ unsigned char pad_1C[0x08];
    /* 0x24 */ struct RenderObjectEntity *animation_source;
    /* 0x28 */ s16 animation_state;
    /* 0x2A */ s16 animation_id;
    /* 0x2C */ u16 table_value2c;
    /* 0x2E */ u16 table_value2e;
    /* 0x30 */ u16 table_value30;
    /* 0x32 */ u16 table_index;
    /* 0x34 */ RenderMatrix model_matrix;
    /* 0x54 */ u8 *primitive_buffer;
    /* 0x58 */ RenderMatrix *active_matrix;
    /* 0x5C */ s16 projected_x;
    /* 0x5E */ s16 projected_y;
    /* 0x60 */ unsigned char pad_60[0x10];
    /* 0x70 */ u16 table_value70;
    /* 0x72 */ unsigned char pad_72[2];
    /* 0x74 */ u16 animation_value74;
    /* 0x76 */ u16 animation_value76;
    /* 0x78 */ u16 animation_value78;
    /* 0x7A */ u16 reserved7a;
    /* 0x7C */ u16 animation_value7c;
    /* 0x7E */ u16 reserved7e;
    /* 0x80 */ RenderAnimationLookupEntry *animation_entries;
    /* 0x84 */ RenderMatrix *matrices;
    /* 0x88 */ unsigned char shade;
    /* 0x89 */ unsigned char pad_89[0x0E];
    /* 0x97 */ u8 script_param97;
    /* 0x98 */ u8 script_param98;
    /* 0x99 */ u8 script_param99;
    /* 0x9A */ s16 script_value9a;
    /* 0x9C */ u16 flags_9C;
    /* 0x9E */ u8 variant_visible;
    /* 0x9F */ unsigned char reserved9f;
    /* 0xA0 */ RenderRotationOverride rotation_overrides[2];
    /* 0xB0 */ void *animation_data;
    /* 0xB4 */ s16 target_x;
    /* 0xB6 */ s16 target_y;
    /* 0xB8 */ s16 target_z;
    /* 0xBA */ signed short draw_count;
} RenderObjectEntity;

PE1_STATIC_ASSERT(sizeof(RenderMatrix) == 0x20, render_matrix_size);
PE1_STATIC_ASSERT(sizeof(RenderPrimitiveDescriptor) == 0x0C,
                  render_primitive_descriptor_size);
PE1_STATIC_ASSERT(sizeof(RenderPacketState) == 0x1C,
                  render_packet_state_prefix_size);
PE1_STATIC_ASSERT(sizeof(RenderPacketValue) == 0x04, render_packet_value_size);
PE1_STATIC_ASSERT(sizeof(RenderPacket34) == 0x34, render_packet34_size);
PE1_STATIC_ASSERT(sizeof(RenderPacket28) == 0x28, render_packet28_size);
PE1_STATIC_ASSERT(sizeof(RenderPacket24) == 0x24, render_packet24_size);
PE1_STATIC_ASSERT(sizeof(RenderPacket1C) == 0x1C, render_packet1c_size);
PE1_STATIC_ASSERT(sizeof(RenderAnimationDataHeader) == 0x0C,
                  render_animation_data_header_size);
PE1_STATIC_ASSERT(sizeof(RenderAnimByteChannel) == 0x04,
                  render_anim_byte_channel_size);
PE1_STATIC_ASSERT(sizeof(RenderAnimShortChannel) == 0x04,
                  render_anim_short_channel_size);
PE1_STATIC_ASSERT(sizeof(RenderRotationOverride) == 0x08,
                  render_rotation_override_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderObjectHeader, packet34_count) == 0x08,
                  render_object_header_primitive_counts_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderMatrix, translation) == 0x14,
                  render_matrix_translation_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderObjectEntity, model_matrix) == 0x34,
                  render_object_model_matrix_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderObjectEntity, matrices) == 0x84,
                  render_object_matrices_offset);
PE1_STATIC_ASSERT(sizeof(RenderAnimationLookupEntry) == 0x0C,
                  render_animation_lookup_entry_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderObjectEntity, animation_data) == 0xB0,
                  render_object_animation_data_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderObjectEntity, target_x) == 0xB4,
                  render_object_target_x_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderObjectEntity, target_z) == 0xB8,
                  render_object_target_z_offset);
PE1_STATIC_ASSERT(sizeof(RenderObjectEntity) == 0xBC, render_object_entity_size);

void Render_SetObjectAnim(RenderObjectEntity *object, RenderObjectEntity *source,
                          short animation_id);
void Render_ClearObjectAnim(RenderObjectEntity *object);
void Render_CopyFrameData(RenderObjectEntity *dst, RenderObjectEntity *src, s32 frame);
void Render_CopyFrameDataDouble(RenderObjectEntity *dst, RenderObjectEntity *src, s32 frame);
void Render_UpdateClutTable(RenderObjectEntity *object, s16 force, s16 buffer_index);
void Anim_DecodeBoneRotationsByte(RenderObjectEntity *object, RenderAnimationDataHeader *animation_data,
                                  s16 frame);
void Anim_DecodeBoneRotationsShort(RenderObjectEntity *object, RenderAnimationDataHeader *animation_data,
                                   s16 frame);

#endif
