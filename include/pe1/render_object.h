#ifndef PE1_RENDER_OBJECT_H
#define PE1_RENDER_OBJECT_H

#include "common.h"
#include "pe1/field_color.h"
#include "pe1/field_sprite_draw.h"
#include "pe1/field_sprite_params.h"
#include "pe1/field_texture_tables.h"
#include "pe1/field_transform_point.h"
#include "pe1/fieldfx_callback_list.h"
#include "pe1/gte_types.h"
#include "pe1/render_matrix_slot.h"

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

void FieldFx_TransformPackedVertex(RenderPackedGeometry *geometry,
                                   GteMatrix *matrix, u16 index,
                                   GteShortVector *out);

/* History entries use halfword XYZ writes when filled, but aligned word
 * copies (including padding) when shifted or replaced. */
typedef union RenderHistoryPoint {
    GteShortVector vector;
    u32 words[2];
} RenderHistoryPoint;

PE1_STATIC_ASSERT(sizeof(RenderHistoryPoint) == 8, render_history_point_size);

void FieldFx_UpdatePointHistory(RenderHistoryPoint *history, s16 count,
                                 RenderHistoryPoint *value, int reset);

typedef struct RenderBouncingSprite {
    s16 x, y, z;
    s16 velocity_x, velocity_y, velocity_z;
    s16 duration, angle;
} RenderBouncingSprite;

PE1_STATIC_ASSERT(sizeof(RenderBouncingSprite) == 16, render_bouncing_sprite_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderBouncingSprite, velocity_x) == 6,
                  render_bouncing_sprite_velocity);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderBouncingSprite, duration) == 12,
                  render_bouncing_sprite_duration);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderBouncingSprite, angle) == 14,
                  render_bouncing_sprite_angle);
int FieldFx_ScatterBurstSprite(int mode, RenderBouncingSprite *state);
int FieldFx_BouncingSpinSprite(int mode, RenderBouncingSprite *state);
int FieldFx_PulsingStarFan(int mode);
extern u8 g_PulsingStarFanColorTrack1[], g_PulsingStarFanColorTrack0[];

typedef struct RenderCosineEffect {
    s16 x;
    s16 amplitude;
    s16 y;
    s16 duration;
} RenderCosineEffect;

PE1_STATIC_ASSERT(sizeof(RenderCosineEffect) == 8, render_cosine_effect_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderCosineEffect, duration) == 6,
                  render_cosine_effect_duration);
int FieldFx_CosineEffectB(int mode, RenderCosineEffect *state);

typedef struct RenderArcingEmitter {
    GteShortVector position;
    int phase;
    int radius;
} RenderArcingEmitter;

PE1_STATIC_ASSERT(sizeof(RenderArcingEmitter) == 0x10,
                  render_arcing_emitter_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderArcingEmitter, radius) == 0xC,
                  render_arcing_emitter_radius);
int FieldFx_ArcingSparkEmitter(int mode, RenderArcingEmitter *state);
extern u8 g_ArcingSparkColorTrack[];

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
int FieldFx_OrbitingSprite(int mode, RenderOrbitingEffect *state);
typedef struct RenderOrbitingEmitter {
    int phase;
    char *particles;
} RenderOrbitingEmitter;

PE1_STATIC_ASSERT(sizeof(RenderOrbitingEmitter) == 8,
                  render_orbiting_emitter_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderOrbitingEmitter, particles) == 4,
                  render_orbiting_emitter_particles);
int FieldFx_OrbitingSpriteBurst(int mode, RenderOrbitingEmitter *state);
extern GteShortVector g_OrbitingEffectAnchor;
extern char *g_OrbitingEffectParticlePool;

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
int FieldFx_VerticalRingSprite(int mode, RenderVerticalEffect *state);
int FieldFx_VerticalEffectEmitter(int mode, void *state);
extern GteShortVector g_VerticalEffectAnchor;
extern u8 g_VerticalEffectColorTrack[];

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
int FieldFx_HelicalTrailPoint(int mode, GteShortVector *state);
int FieldFx_HelicalSprite(int mode, RenderHelicalEffect *state);
int FieldFx_HelicalSpriteEmitter(int mode, RenderOrbitingEmitter *state);
extern u8 g_HelicalTrailColorTrack[];
extern GteShortVector g_HelicalEffectAnchor;
extern char *g_HelicalEffectParticlePool;

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
int FieldFx_DampedSpark(int mode, RenderDampedSpark *state);
int FieldFx_DampedSparkEmitter(int mode, RenderSparkEmitter *state);
extern u8 g_DampedSparkColorTrack[];

typedef struct RenderArcingEffect {
    s16 x, y, z;
    s16 velocity_y;
} RenderArcingEffect;

PE1_STATIC_ASSERT(sizeof(RenderArcingEffect) == 8, render_arcing_effect_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderArcingEffect, velocity_y) == 6,
                  render_arcing_effect_velocity);
int FieldFx_ArcingSpark(int mode, RenderArcingEffect *state);
int FieldFx_RisingSpinSprite(int mode, RenderArcingEffect *state);
int FieldFx_DescendingSprite(int mode, RenderArcingEffect *state);
int FieldFx_DescendingSpriteEmitter(int mode, GteShortVector *state);
int FieldFx_RisingSpinEmitter(int mode, GteShortVector *state);
extern u8 g_ArcingEffectColorTrack[];

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

/* The sine and spark modes use the same runtime emitter record. */
typedef RenderSparkEmitter RenderSineEmitter;

PE1_STATIC_ASSERT(sizeof(RenderSineEmitter) == 0xC, render_sine_emitter_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderSineEmitter, phase) == 8,
                  render_sine_emitter_phase);
int FieldFx_SineEmitter(int mode, RenderSineEmitter *state);

extern u8 g_SineSweepColorTrack[];
extern GteShortVector g_SineEffectAnchor;
int FieldFx_SineSweepTriangle(int mode, RenderSineEffect *state);

extern u8 g_CosineEffectColorTrack[];
extern int g_SparkEmitterPhaseSeed;
extern u8 g_RingFlashColorTrack[], g_RingFlashInnerColorTrack[];
int FieldFx_RingFlash(int mode, RenderSparkEmitter *state);
extern u8 g_JitterSpriteColorTrack[];
extern u8 g_RisingJitterColorTrack[];
int FieldFx_RisingJitterSprite(int mode, GteShortVector *state);
int FieldFx_RisingJitterEmitter(int mode, GteShortVector *state);
int FieldFx_JitterSprite(int mode, GteShortVector *state);
int FieldFx_JitterSpriteEmitter(int mode, GteShortVector *state);
int FieldFx_CosineEffect(int mode, RenderCosineEffect *effect);

extern u8 g_RotatingEffectColorTrack[];
int FieldFx_RotatingEffect(int mode);

extern RenderColor g_FieldFxGlowFountainTint;
extern RenderColor g_FieldFxCosineEffectTint;
extern RenderColor g_FieldFxArcingEmitterTint;
extern u8 g_CosineSparkColorTrack[];
extern RenderColor g_FieldFxBouncingSpriteTint;

typedef struct RenderDiamondEmitter {
    GteShortVector position;
    int x, y, angle, size, radius;
} RenderDiamondEmitter;
extern GteShortVector g_DiamondParticleAnchor;
extern u8 g_DiamondParticleColorTrack[];
typedef struct RenderDiamondParticle {
    s16 x, y, angle, size, reserved, color_time;
} RenderDiamondParticle;
PE1_STATIC_ASSERT(sizeof(RenderDiamondEmitter) == 28, render_diamond_emitter_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderDiamondEmitter, radius) == 24,
                  render_diamond_emitter_radius);
PE1_STATIC_ASSERT(sizeof(RenderDiamondParticle) == 12, render_diamond_particle_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderDiamondParticle, color_time) == 10,
                  render_diamond_particle_color_time);
int FieldFx_DiamondParticle(int mode, RenderDiamondParticle *state);
int FieldFx_DiamondEmitter(int mode, RenderDiamondEmitter *state);

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
extern GteShortVector g_SettlingSpriteAnchor;
extern u8 g_SettlingEmitterColorTrack[];
extern u8 g_SettlingSpriteColorTrack[];
int FieldFx_SettlingSprite(int mode, RenderSettlingSprite *state);
int FieldFx_SettlingEmitter(int mode, GteShortVector *state);

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
extern GteShortVector g_ConvergingSpriteAnchor;
extern u8 g_ConvergingSpriteColorTrack[];
int FieldFx_ConvergingSprite(int mode, RenderConvergingSprite *state);
int FieldFx_ConvergingEmitter(int mode, RenderConvergingEmitter *state);

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
extern u8 g_TiltingSpriteColorTrack[];
extern GteShortVector g_TiltingSpriteAnchor;
int FieldFx_TiltingSprite(int mode, RenderTiltingSprite *state);
int FieldFx_TiltingSpriteEmitter(int mode, RenderSparkEmitter *state);

extern RenderColor g_FieldFxArcingSparkTint;
extern RenderColor g_FieldFxOrbitHelicalTint;
extern RenderColor g_FieldFxDampedSparkTint;
extern RenderColor g_FieldFxSparkOuterTint;
extern RenderColor g_FieldFxSparkInnerTint;
int FieldFx_CosineEmitter(int mode, RenderSparkEmitter *state);


void LoadAverageCol(void *first, void *second, int first_scale,
                    int second_scale, void *output);


extern u8 g_OrbitTrailColorTrack[];
extern u8 g_DriftingEffectColorTrack[];
int FieldFx_DriftingEffect(int mode, GteShortVector *position);
int FieldFx_DriftingEmitter(int mode, GteShortVector *position);
extern u8 g_StarFanRingGlowColorTrack[];
extern u8 g_StarFanRingGlowSpinColorTrack[];
int FieldFx_StarFanRingGlow(int mode, RenderSparkEmitter *state);
int FieldFx_OrbitTrailPoint(int mode, GteShortVector *state);


void FieldFx_TransformTranslation(const GteShortVector *input,
                                  GteMatrixWords *output);

extern GteShortVector D_800C2258, g_FieldFxZeroVector;
extern GteShortVector D_800C1D18;

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
    unsigned char pad_05[3];
    /* Supplies the Z component of the non-root matrix translation vector. */
    s32 translation_z;
} RenderObjectPart;

PE1_STATIC_ASSERT(sizeof(RenderObjectPart) == 12, render_object_part_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderObjectPart, translation_z) == 8,
                  render_object_part_translation_z);

typedef struct RenderObjectHeader {
    unsigned short prim_block_count; /* 8-byte blocks added to a full allocation */
    unsigned char part_count;
    unsigned char animation_entry_count;
    unsigned char pad_04[2];
    unsigned short vertex_count;
    unsigned short packet34_count;
    unsigned short packet28_count;
    unsigned short packet24_count;
    unsigned short packet1c_count;
    /* Input Y and matrix selection for the transformed anchor position. */
    u16 anchor_y;
    u16 anchor_matrix_index;
    u16 shadow_radius; /* 0x14: half extent of the ground shadow quad */
    u16 scale;         /* 0x16: copied from the owning actor's move_speed */
    unsigned short matrix_command_bytes;
    unsigned short visible_part_count;
} RenderObjectHeader;

typedef struct RenderPrimitiveDescriptor {
    /* 0x00 */ unsigned char reserved00[3];
    /* 0x03 */ u8 kind;
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
    /* 0x08 */ u32 sxy0;
    /* 0x0C */ u8 u0, v0;
    /* 0x0E */ u16 clut;
    /* 0x10 */ u32 value1;
    /* 0x14 */ u32 sxy1;
    /* 0x18 */ u8 u1, v1;
    /* 0x1A */ u16 page_bits;
    /* 0x1C */ u32 value2;
    /* 0x20 */ u32 sxy2;
    /* 0x24 */ u8 u2, v2;
    /* 0x26 */ u16 reserved26;
    /* 0x28 */ u32 value3;
    /* 0x2C */ u32 sxy3;
    /* 0x30 */ u8 u3, v3;
    /* 0x32 */ u16 reserved32;
} RenderPacket34;

typedef struct RenderPacket28 {
    /* 0x00 */ u32 tag;
    /* 0x04 */ RenderPacketValue values0;
    /* 0x08 */ u32 sxy0;
    /* 0x0C */ u8 u0, v0;
    /* 0x0E */ u16 clut;
    /* 0x10 */ u32 value1;
    /* 0x14 */ u32 sxy1;
    /* 0x18 */ u8 u1, v1;
    /* 0x1A */ u16 page_bits;
    /* 0x1C */ u32 value2;
    /* 0x20 */ u32 sxy2;
    /* 0x24 */ u8 u2, v2;
    /* 0x26 */ u16 reserved26;
} RenderPacket28;

typedef struct RenderPacket24 {
    /* 0x00 */ u32 tag;
    /* 0x04 */ RenderPacketValue values0;
    /* 0x08 */ u32 sxy0;
    /* 0x0C */ u32 value1;
    /* 0x10 */ u32 sxy1;
    /* 0x14 */ u32 value2;
    /* 0x18 */ u32 sxy2;
    /* 0x1C */ u32 value3;
    /* 0x20 */ u32 sxy3;
} RenderPacket24;

typedef struct RenderPacket1C {
    /* 0x00 */ u32 tag;
    /* 0x04 */ RenderPacketValue values0;
    /* 0x08 */ u32 sxy0;
    /* 0x0C */ u32 value1;
    /* 0x10 */ u32 sxy1;
    /* 0x14 */ u32 value2;
    /* 0x18 */ u32 sxy2;
} RenderPacket1C;

/* Hit sphere keyed by a part id: Render_FindAnimEntry looks one up by id, and
 * Scene_UpdateEntityPositions tests every pair of two actors' spheres. The
 * render object's own cylinder (+0x68) and body (+0x74) spheres use the same
 * layout. Render paths treat the radius and centre as unsigned 16-bit values. */
typedef struct RenderAnimationLookupEntry {
    /* 0x00 */ s16 value0;          /* centre x */
    /* 0x02 */ s16 value1;          /* centre y */
    /* 0x04 */ s16 value2;          /* centre z */
    /* 0x06 */ s16 animation_id;    /* part id passed to the contact callback */
    /* 0x08 */ s16 radius;          /* unscaled; multiplied by move_speed / 4096 */
    /* 0x0A */ s16 bounds_value1;   /* second bounds word (Render_SetupEntityPrims) */
} RenderAnimationLookupEntry;

typedef struct RenderAnimationDataHeader {
    /* 0x00 */ u8 encoding_flags;
    /* 0x01 */ u8 last_bone_index;
    /* 0x02 */ u8 frame_count;  /* samples per channel; sets the channel stride */
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
    /* 0x1C */ RenderVec3s *projection_origin;
    /* 0x20 */ s8 *matrix_commands;
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
    /* 0x60 */ unsigned char pad_60[4];
    /* 0x64 */ s16 projected_target_x, projected_target_y;
    /* 0x68 */ RenderAnimationLookupEntry hit_cylinder; /* anchor; x/z cylinder test */
    /* 0x74 */ RenderAnimationLookupEntry hit_body;     /* world position; body sphere test */
    /* 0x80 */ RenderAnimationLookupEntry *animation_entries;
    /* 0x84 */ RenderMatrix *matrices;
    /* 0x88 */ unsigned char shade;
    /* 0x89 */ u8 lightNegativeY;
    /* 0x8A */ u8 lightPositiveY;
    /* 0x8B */ u8 reserved8b;
    /* 0x8C */ s8 fade_remaining, fade_duration;
    /* 0x8E */ u8 fade_red_step, fade_green_step;
    /* 0x90 */ u8 primitive_red, primitive_green, primitive_blue;
    /* 0x93 */ u8 fade_blue_step;
    /* 0x94 */ u8 fade_red, fade_green, fade_blue;
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
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderPrimitiveDescriptor, kind) == 3,
                  render_primitive_descriptor_kind_offset);
PE1_STATIC_ASSERT(sizeof(RenderPrimitiveDescriptor) == 0x0C,
                  render_primitive_descriptor_size);
PE1_STATIC_ASSERT(sizeof(RenderPacketState) == 0x1C,
                  render_packet_state_prefix_size);
PE1_STATIC_ASSERT(sizeof(RenderPacketValue) == 0x04, render_packet_value_size);
PE1_STATIC_ASSERT(sizeof(RenderPacket34) == 0x34, render_packet34_size);
PE1_STATIC_ASSERT(sizeof(RenderPacket28) == 0x28, render_packet28_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderPacket34, clut) == 0x0E, render_packet34_clut);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderPacket34, u1) == 0x18, render_packet34_uv1);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderPacket34, u2) == 0x24, render_packet34_uv2);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderPacket34, v3) == 0x31, render_packet34_v3);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderPacket28, clut) == 0x0E, render_packet28_clut);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderPacket28, u1) == 0x18, render_packet28_uv1);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderPacket28, u2) == 0x24, render_packet28_uv2);
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
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderObjectEntity, primitive_red) == 0x90,
                  render_object_primitive_red_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderObjectEntity, primitive_green) == 0x91,
                  render_object_primitive_green_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderObjectEntity, primitive_blue) == 0x92,
                  render_object_primitive_blue_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderObjectEntity, fade_remaining) == 0x8C,
                  render_object_fade_remaining_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderObjectEntity, fade_duration) == 0x8D,
                  render_object_fade_duration_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderObjectEntity, fade_red_step) == 0x8E,
                  render_object_fade_red_step_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderObjectEntity, fade_green_step) == 0x8F,
                  render_object_fade_green_step_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderObjectEntity, fade_blue_step) == 0x93,
                  render_object_fade_blue_step_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderObjectEntity, fade_red) == 0x94,
                  render_object_fade_red_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderObjectEntity, fade_green) == 0x95,
                  render_object_fade_green_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderObjectEntity, fade_blue) == 0x96,
                  render_object_fade_blue_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderObjectHeader, anchor_y) == 0x10,
                  render_object_header_anchor_y);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderObjectHeader, anchor_matrix_index) == 0x12,
                  render_object_header_anchor_matrix_index);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderObjectEntity, projection_origin) == 0x1C,
                  render_object_projection_origin);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderObjectEntity, projected_target_x) == 0x64,
                  render_object_projected_target);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderObjectEntity, hit_cylinder) == 0x68,
                  render_object_hit_cylinder);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderObjectEntity, hit_cylinder.radius) == 0x70,
                  render_object_hit_cylinder_radius);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderObjectEntity, hit_body) == 0x74,
                  render_object_hit_body);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderObjectEntity, hit_body.radius) == 0x7C,
                  render_object_hit_body_radius);
PE1_STATIC_ASSERT(sizeof(RenderObjectEntity) == 0xBC, render_object_entity_size);

union RenderLightingMatrix;
void Render_DrawObject(RenderObjectEntity *object, union RenderLightingMatrix *matrix);
void Render_DrawObjectVariant(RenderObjectEntity *object, s16 limit, s32 *projectionMatrix);
void Render_DrawObjectAlt(RenderObjectEntity *object, s16 limit, u8 red, u8 green, u8 blue);
void Render_DrawTexturedQuads(RenderObjectEntity *object);
void Render_BuildEntityMatrix(RenderObjectEntity *object, s16 bufferIndex);
extern u32 g_RenderFadeColour;
int Render_TickObject(RenderObjectEntity *object);
int Render_ColorEntity(RenderObjectEntity *object);
/* Texture-page bit deltas, indexed by the current and the requested page. */
extern s16 g_RenderTexturePageDeltas[4][4];
/* Text-resident word bumped by Render_IncrementCounter. Unsized: the word
 * lives in .text, so it is not small data and -G8 code keeps its address
 * in a register, as retail does. */
extern int g_RenderCounter[];
void Render_IncrementCounter(void);
void Render_SetObjectTexturePage(RenderObjectEntity *object, int page);
void Render_SetEntityBlendMode(RenderObjectEntity *object, int mode);
void Render_FadeEntityColor(RenderObjectEntity *object, int r, int g, int b);
void Render_OffsetObjectTextureCoordinates(RenderObjectEntity *object,
                                           int du, int dv, int clutOffset);
void Render_SetObjectAnim(RenderObjectEntity *object, RenderObjectEntity *source,
                          short animation_id);
void Render_ClearObjectAnim(RenderObjectEntity *object);
void Render_SetupBoneTransforms(RenderObjectEntity *object, s32 *view_matrix);
void Render_TransformVertices(RenderObjectEntity *object);
void Render_TransformSkinnedVertices(RenderObjectEntity *object, u32 *view_matrix);
void Render_TransformMorphVertices(RenderObjectEntity *object, u32 *view_matrix);
/* Word lookup/work buffers also used for GTE screen/depth output. */
extern u32 g_RenderVertexWorkWords[], g_RenderClutLookupTable[];
/* Unit vertex normals indexed by RenderVec3s.pad of a model vertex. */
extern RenderVec3s g_RenderNormalTable[];
/* Projected screen XY and view depth of the transformed model vertices. */
extern u32 D_800B1640[];
extern u32 D_800B1100[];
extern u32 D_800B1644[];
extern u32 D_800B1104[];
extern s32 D_800A636C[];
extern s32 D_800A5E2C[];
extern u8 g_SkinAnchorVector[];
extern s16 g_SkinAnchorY;
void Render_CopyFrameData(RenderObjectEntity *dst, RenderObjectEntity *src, s32 frame);
void Render_CopyFrameDataDouble(RenderObjectEntity *dst, RenderObjectEntity *src, s32 frame);
void Render_UpdateClutTable(RenderObjectEntity *object, s16 force, s16 buffer_index);
void Anim_DecodeBoneRotationsByte(RenderObjectEntity *object, RenderAnimationDataHeader *animation_data,
                                  s16 frame);
void Anim_DecodeBoneRotationsShort(RenderObjectEntity *object, RenderAnimationDataHeader *animation_data,
                                   s16 frame);

void Render_AnimationFrame(void);

/* Packed screen coordinates written by Render_DrawTexturedQuads. */
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderPacket34, sxy0) == 0x08, render_packet34_sxy0);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderPacket34, sxy1) == 0x14, render_packet34_sxy1);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderPacket34, sxy2) == 0x20, render_packet34_sxy2);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderPacket34, sxy3) == 0x2C, render_packet34_sxy3);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderPacket28, sxy0) == 0x08, render_packet28_sxy0);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderPacket28, sxy1) == 0x14, render_packet28_sxy1);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderPacket28, sxy2) == 0x20, render_packet28_sxy2);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderPacket24, sxy0) == 0x08, render_packet24_sxy0);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderPacket24, sxy1) == 0x10, render_packet24_sxy1);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderPacket24, sxy2) == 0x18, render_packet24_sxy2);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderPacket24, sxy3) == 0x20, render_packet24_sxy3);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderPacket1C, sxy0) == 0x08, render_packet1c_sxy0);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderPacket1C, sxy1) == 0x10, render_packet1c_sxy1);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderPacket1C, sxy2) == 0x18, render_packet1c_sxy2);

/* Position and velocity words wrap at 16 bits in this particle update. */
typedef struct RenderRisingEffect {
    u16 x, y, z;
    u16 vx, vy, vz;
    u16 angle;
} RenderRisingEffect;
PE1_STATIC_ASSERT(sizeof(RenderRisingEffect) == 14, render_rising_effect_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderRisingEffect, angle) == 12,
                  render_rising_effect_angle);
extern u8 g_RisingSpriteColorTrack[];
int FieldFx_RisingSprite(int mode, RenderRisingEffect *state);
int FieldFx_RisingSpriteEmitter(int mode, RenderArcingEmitter *state);

/* Sprites circling the spiral anchor: 0x800D6514 spawns 24 of them for
 * 0x800D629C and publishes the anchor and the circle radius each frame. */
typedef struct RenderSpiralSprite {
    GteShortVector position;
    s16 angle, height, phase, reserved0E;
} RenderSpiralSprite;
PE1_STATIC_ASSERT(sizeof(RenderSpiralSprite) == 16, render_spiral_sprite_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderSpiralSprite, phase) == 12,
                  render_spiral_sprite_phase);
extern u16 g_SpiralEffectRadius;
extern GteShortVector g_SpiralEffectAnchor;
extern RenderColor g_FieldFxSpiralSpriteTint;
extern RenderColor g_FieldFxRingPulseTint;
int FieldFx_SpiralSprite(int mode, RenderSpiralSprite *state);
int FieldFx_SpiralEmitter(int mode, RenderArcingEmitter *state);

typedef struct RenderRingPulse {
    GteShortVector position;
    GteShortVector raised;
    int phase;
} RenderRingPulse;
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderRingPulse, phase) == 16,
                  render_ring_pulse_phase);
int FieldFx_RingPulse(int mode, RenderRingPulse *state);

/* Falling sprite pair: 0x800D4C24 drops one sprite per frame for 0x800D4928
 * from above the player, tinted by the scene's colour track. */
typedef struct RenderPhaseEmitter {
    int phase;
} RenderPhaseEmitter;
extern u8 g_FallingSpriteColorTrack0[];
extern u8 g_FallingSpriteColorTrack1[];
extern u8 g_BouncingSpriteColorTrack2[];
int FieldFx_FallingSprite(int mode, RenderBouncingSprite *state);
int FieldFx_FallingSpriteEmitter(int mode, RenderPhaseEmitter *state);

typedef struct RenderFadeEmitter {
    s16 count, intensity;
} RenderFadeEmitter;
typedef struct RenderFadeParticle {
    s16 position[3];
    s16 kind;
    s16 vx, vy, vz;
    s16 reserved0E;
    s16 phase, timer;
} RenderFadeParticle;
PE1_STATIC_ASSERT(sizeof(RenderFadeEmitter) == 4, render_fade_emitter_size);
PE1_STATIC_ASSERT(sizeof(RenderFadeParticle) == 20, render_fade_particle_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderFadeParticle, kind) == 6, render_fade_particle_kind);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderFadeParticle, phase) == 16, render_fade_particle_phase);
extern u8 g_FadeEmitterParticleKinds[];
/* Intensity cap the emitter publishes for its fade particles. */
extern s16 g_FadeEmitterIntensity;
/* Offset of a fade particle from its actor matrix point, and the colour
 * track of its falling sparks. */
extern GteShortVector g_FieldFxZeroOffset;
extern u8 g_FadeParticleColorTrack[];
int FieldFx_FadeParticle(int mode, RenderFadeParticle *particle);
int FieldFx_FadeEmitter(int mode, RenderFadeEmitter *state);

extern RenderObjectEntity g_PlayerBattleEntity;

void Render_InitObjectFromTable(RenderObjectEntity *object,
                                RenderObjectEntity *source, int index);

#endif
