#ifndef PE1_FIELD_RING_GEOMETRY_H
#define PE1_FIELD_RING_GEOMETRY_H

#include "common.h"

/* Shared 24-byte record used by room emitters and the field ring builder /
 * renderer. The room-side names describe the producer view; in the field
 * ring view, mode is the point count, extent0/1 are inner/outer radii, and
 * offset/intensity are depth/brightness. */
typedef struct FieldRingGeometry {
    /* 0x00 */ void *source;
    /* 0x04 */ u8 color0[3];
    /* 0x07 */ u8 pad07;
    /* 0x08 */ u8 color1[3];
    /* 0x0B */ u8 pad0B;
    /* 0x0C */ u16 mode;
    /* 0x0E */ u16 extent0;
    /* 0x10 */ u16 extent1;
    /* 0x12 */ s16 offset;
    /* 0x14 */ s16 intensity;
    /* 0x16 */ s16 pad16;
} FieldRingGeometry;

typedef FieldRingGeometry RoomFxEmitterParams;

PE1_STATIC_ASSERT(PE1_OFFSETOF(FieldRingGeometry, source) == 0,
                  field_ring_geometry_source_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(FieldRingGeometry, color0) == 4,
                  field_ring_geometry_color0_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(FieldRingGeometry, color1) == 8,
                  field_ring_geometry_color1_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(FieldRingGeometry, mode) == 0x0C,
                  field_ring_geometry_mode_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(FieldRingGeometry, extent0) == 0x0E,
                  field_ring_geometry_extent0_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(FieldRingGeometry, extent1) == 0x10,
                  field_ring_geometry_extent1_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(FieldRingGeometry, offset) == 0x12,
                  field_ring_geometry_offset_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(FieldRingGeometry, intensity) == 0x14,
                  field_ring_geometry_intensity_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(FieldRingGeometry, pad16) == 0x16,
                  field_ring_geometry_pad16_offset);
PE1_STATIC_ASSERT(sizeof(FieldRingGeometry) == 0x18,
                  field_ring_geometry_size);

#endif /* PE1_FIELD_RING_GEOMETRY_H */
