#ifndef PE1_FIELD_TILE_H
#define PE1_FIELD_TILE_H

#include "common.h"
#include "pe1/gte_types.h"
#include "pe1/render_prim.h"
#include "pe1/render_tint.h"

/* Flat colour tiles linked straight into the active ordering table. */

/* An ordering-table entry or packet seen both as its tag and as the 32-bit
 * address the GPU DMA chain links through. */
typedef union FieldTileAddress {
    RenderGpuTag *tag;
    char *bytes;
    u32 word;
} FieldTileAddress;

/* Entry `depth` of the table held in `base`: retail adds the scaled depth
 * to the table address as an integer, index first. */
#define TILE_OT_ENTRY(entry, base, table, depth) \
    ((base).bytes = (table), (entry).word = (depth) * 4 + (base).word)

/* PSY-Q addPrim: link `packet` (with a RenderGpuTag tag) in front of the
 * chain at `entry`, storing the low 24 address bits in each tag. */
#define TILE_OT_ADDPRIM(entry, packet, link) \
    ((packet)->tag.address = (entry).tag->address, \
     (link).tag = &(packet)->tag, \
     (entry).tag->address = (link).word)

/* One-pixel TILE_1 packet. */
typedef struct FieldTilePoint {
    RenderGpuTag tag;
    u8 r, g, b, command;
    s16 x, y;
} FieldTilePoint;

/* Matches render_object.h; func_800D1DEC's unit does not include it because
 * that header declares the function with untyped pointers. */
extern s16 D_800F3374;

void *memset(void *dst, int value, unsigned int size);
void SetTile(RenderTintTile *tile);
void Gpu_SetDither(void *packet, int dither);
void AddPrim(unsigned int *orderingEntry, unsigned int *primitive);
void SetTile1(FieldTilePoint *tile);

void func_800D1AE0(u8 *color, int scale, int abr, u32 depth);
void func_800D2104(GteShortVector *position, u8 *color, int scale, int abr);

#endif
