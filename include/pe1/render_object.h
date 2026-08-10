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
    unsigned char pad_04[0x16];
    unsigned short visible_part_count;
} RenderObjectHeader;

typedef struct RenderAnimationLookupEntry {
    /* 0x00 */ s16 value0;
    /* 0x02 */ s16 value1;
    /* 0x04 */ s16 value2;
    /* 0x06 */ s16 animation_id;
    /* 0x08 */ unsigned char reserved08[4];
} RenderAnimationLookupEntry;

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
    /* 0x10 */ unsigned char pad_10[0x08];
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
    /* 0x54 */ void *skeleton_data;
    /* 0x58 */ void *skeleton_aux;
    /* 0x5C */ s16 projected_x;
    /* 0x5E */ s16 projected_y;
    /* 0x60 */ unsigned char pad_60[0x10];
    /* 0x70 */ u16 table_value70;
    /* 0x72 */ unsigned char pad_72[0x0E];
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
    /* 0x9F */ unsigned char pad_9F[0x11];
    /* 0xB0 */ void *animation_data;
    /* 0xB4 */ s16 target_x;
    /* 0xB6 */ s16 target_y;
    /* 0xB8 */ s16 target_z;
    /* 0xBA */ signed short draw_count;
} RenderObjectEntity;

PE1_STATIC_ASSERT(sizeof(RenderMatrix) == 0x20, render_matrix_size);
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

#endif
