#ifndef PE1_COLLISION_DATABASE_H
#define PE1_COLLISION_DATABASE_H

#include "common.h"

/* Packed vertex tables used by the flat and sloped triangle formats.
 * Coordinates are loaded as halfwords and interpreted as signed for geometry. */
typedef struct CollisionVertexXZ {
    u16 x, z;
} CollisionVertexXZ;
typedef struct CollisionVertexXYZ {
    u16 x, y, z;
} CollisionVertexXYZ;
/* The vertex table pointer, also read as the 32-bit address retail adds
 * the scaled vertex index to (index first). */
typedef union CollisionVertexTable {
    void *pointer;
    CollisionVertexXZ *xz;
    CollisionVertexXYZ *xyz;
    u32 word;
} CollisionVertexTable;
/* The first word is byte-addressed in both triangle formats. Keeping this
 * two-byte view separate from the sloped face also describes the 22-byte
 * flat records, whose addresses need not be aligned to a 32-bit word. */
typedef struct CollisionTriangleHeader {
    u8 kind;
    u8 region; /* flat mode: index into the height records */
} CollisionTriangleHeader;

/* Sloped triangle prefix. Only kind and region are shared with flat triangles. */
typedef struct CollisionFace {
    u8 kind;
    u8 region;    /* flat mode: index into D_8009CE08 */
    u16 plane;    /* sloped mode: index into D_8009D1D8 */
    s32 distance; /* sloped mode: plane D term */
} CollisionFace;

/* Walkable triangles. Both formats are walked as halfword arrays: the
 * flat one keeps its vertex indices at [1..3] and its edge neighbours at
 * [7..9], the sloped one at [4..6] and [10..12]. */
typedef union CollisionTriangleXZ {
    CollisionTriangleHeader header;
    u16 words[11];
    u8 kind; /* bit 0x80: not walkable from a neighbour */
} CollisionTriangleXZ;
typedef union CollisionTriangleXYZ {
    CollisionTriangleHeader header;
    CollisionFace face;
    u16 words[14];
    u8 kind;
} CollisionTriangleXYZ;
PE1_STATIC_ASSERT(PE1_OFFSETOF(CollisionFace, plane) == 2, collision_face_plane_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(CollisionFace, distance) == 4, collision_face_distance_offset);

typedef union CollisionTriangleTable {
    void *pointer;
    CollisionTriangleHeader *header; /* Common prefix of either triangle format. */
    CollisionTriangleXZ *xz;
    CollisionTriangleXYZ *xyz;
    u32 word;
} CollisionTriangleTable;
/* Per-region triangle index lists; the database selects one height mode. */
typedef struct CollisionFlatRegion {
    s16 height;
    u16 triangleCount;
    u16 triangleIndices[0];
} CollisionFlatRegion;
typedef struct CollisionPlaneRegion {
    u32 reserved;
    u16 triangleCount;
    u16 triangleIndices[0];
} CollisionPlaneRegion;

PE1_STATIC_ASSERT(PE1_OFFSETOF(CollisionFlatRegion, triangleIndices) == 4,
                  collision_flat_region_indices_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(CollisionPlaneRegion, triangleIndices) == 6,
                  collision_plane_region_indices_offset);

typedef struct CollisionDatabase {
    u16 reserved00;
    u16 regionCount;                  /* 0x02: inline height-table references. */
    u32 reserved04;
    u16 visitedWordCount;             /* 0x08: triangle count on disk; bitmap words after relocation. */
    u16 reserved0A;
    u32 reserved0C[3];
    CollisionVertexTable vertices;   /* 0x18: XZ or XYZ array. */
    CollisionTriangleTable triangles; /* 0x1C: XZ or XYZ triangles. */
    union { struct CollisionPlane *pointer; u32 word; } planes; /* 0x20 */
    union { struct RampEdge *pointer; u32 word; } rampEdges; /* 0x24 */
    union { s16 *height; u32 word; } regions[0]; /* 0x28: variable-length tail. */
} CollisionDatabase;


extern CollisionDatabase *g_CollisionDb;
/* Active views published by the scene-data relocator. */
extern struct CollisionPlane *g_CollisionPlaneTable;
extern s16 **g_RegionHeightTable;

PE1_STATIC_ASSERT(sizeof(CollisionTriangleHeader) == 2, collision_triangle_header_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(CollisionTriangleHeader, region) == 1,
                  collision_triangle_region_offset);

PE1_STATIC_ASSERT(sizeof(CollisionDatabase) == 0x28, collision_database_header_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(CollisionDatabase, regions) == 0x28,
                  collision_database_regions_offset);

#endif
