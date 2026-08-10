#ifndef PE1_RENDER_OBJECT_H
#define PE1_RENDER_OBJECT_H

#include "common.h"

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
PE1_STATIC_ASSERT(sizeof(RenderAnimationDataHeader) == 0x0C,
                  render_animation_data_header_size);
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
PE1_STATIC_ASSERT(sizeof(RenderObjectEntity) == 0xBC, render_object_entity_size);

void Render_SetObjectAnim(RenderObjectEntity *object, RenderObjectEntity *source,
                          short animation_id);
void Render_ClearObjectAnim(RenderObjectEntity *object);
void Render_CopyFrameData(RenderObjectEntity *dst, RenderObjectEntity *src, s32 frame);
void Render_CopyFrameDataDouble(RenderObjectEntity *dst, RenderObjectEntity *src, s32 frame);
void Render_UpdateClutTable(RenderObjectEntity *object, s16 force, s16 buffer_index);
void Anim_DecodeBoneRotationsByte(RenderObjectEntity *object, void *animation_data,
                                  s16 frame);
void Anim_DecodeBoneRotationsShort(RenderObjectEntity *object, void *animation_data,
                                   s16 frame);

#endif
