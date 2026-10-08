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
    u16 words[14];
    u8 kind;
} CollisionTriangleXYZ;
typedef union CollisionTriangleTable {
    void *pointer;
    CollisionTriangleXZ *xz;
    CollisionTriangleXYZ *xyz;
    u32 word;
} CollisionTriangleTable;
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

PE1_STATIC_ASSERT(sizeof(CollisionTriangleHeader) == 2, collision_triangle_header_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(CollisionTriangleHeader, region) == 1,
                  collision_triangle_region_offset);

PE1_STATIC_ASSERT(sizeof(CollisionDatabase) == 0x28, collision_database_header_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(CollisionDatabase, regions) == 0x28,
                  collision_database_regions_offset);

#endif
