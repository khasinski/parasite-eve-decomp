#ifndef FX_COMMON_MODEL_H
#define FX_COMMON_MODEL_H

#include "fx_common_render.h"
#include "fx_common_motion.h"

/* Polygon model blocks: a header of eight primitive counts and eight byte
 * offsets (relative to the block) of the primitive arrays, one per kind. */
union FxCommonPolyModel {
    FxCommonPolyResource header;
    s32 bankOffsets[1];
    u8 bytes[1];
};

/* Model primitives.  Colours keep the GPU command byte in their top byte;
 * texture words are stored as the packet expects them (uv + clut/tpage), the
 * quads keeping uv2 and uv3 as halves of one word. */
typedef struct FxCommonPacketRgb {
    u8 r, g, b, code;
} FxCommonPacketRgb;

typedef union FxCommonModelColor {
    u32 word;
    FxCommonPacketRgb rgb;
} FxCommonModelColor;

typedef union FxCommonModelUvPair {
    u32 word;
    struct {
        u16 uv2;
        u16 uv3;
    } halves;
} FxCommonModelUvPair;

typedef struct FxCommonModelF3 {
    FxCommonModelColor color;
    FxCommonVector vertices[3];
} FxCommonModelF3;

typedef struct FxCommonModelF4 {
    FxCommonModelColor color;
    FxCommonVector vertices[4];
} FxCommonModelF4;

typedef struct FxCommonModelG3 {
    FxCommonModelColor colors[3];
    FxCommonVector vertices[3];
} FxCommonModelG3;

typedef struct FxCommonModelG4 {
    FxCommonModelColor colors[4];
    FxCommonVector vertices[4];
} FxCommonModelG4;

typedef struct FxCommonModelFt3 {
    FxCommonModelColor color;
    u32 uv0;
    u32 uv1;
    u32 uv2;
    FxCommonVector vertices[3];
} FxCommonModelFt3;

typedef struct FxCommonModelFt4 {
    FxCommonModelColor color;
    u32 uv0;
    u32 uv1;
    FxCommonModelUvPair uv23;
    FxCommonVector vertices[4];
} FxCommonModelFt4;

typedef struct FxCommonModelGt3 {
    FxCommonModelColor colors[3];
    u32 uv0;
    u32 uv1;
    u32 uv2;
    FxCommonVector vertices[3];
} FxCommonModelGt3;

typedef struct FxCommonModelGt4 {
    FxCommonModelColor colors[4];
    u32 uv0;
    u32 uv1;
    FxCommonModelUvPair uv23;
    FxCommonVector vertices[4];
} FxCommonModelGt4;

/* GPU packets written by the model drawers.  Colour and texture words are
 * copied whole from the model, in the PSY-Q style of storing a long over the
 * byte fields. */
typedef struct FxCommonPacketUv {
    u8 u, v;
    u16 page;
} FxCommonPacketUv;

typedef struct FxCommonModelF3Packet {
    FxCommonPacketTag tag;
    FxCommonPacketRgb rgb;
    u32 xy0, xy1, xy2;
} FxCommonModelF3Packet;

typedef struct FxCommonModelF4Packet {
    FxCommonPacketTag tag;
    FxCommonPacketRgb rgb;
    u32 xy0, xy1, xy2, xy3;
} FxCommonModelF4Packet;

typedef struct FxCommonModelG3Packet {
    FxCommonPacketTag tag;
    FxCommonPacketRgb rgb0;
    u32 xy0;
    FxCommonPacketRgb rgb1;
    u32 xy1;
    FxCommonPacketRgb rgb2;
    u32 xy2;
} FxCommonModelG3Packet;

typedef struct FxCommonModelG4Packet {
    FxCommonPacketTag tag;
    FxCommonPacketRgb rgb0;
    u32 xy0;
    FxCommonPacketRgb rgb1;
    u32 xy1;
    FxCommonPacketRgb rgb2;
    u32 xy2;
    FxCommonPacketRgb rgb3;
    u32 xy3;
} FxCommonModelG4Packet;

typedef struct FxCommonModelFt3Packet {
    FxCommonPacketTag tag;
    FxCommonPacketRgb rgb;
    u32 xy0;
    FxCommonPacketUv uv0;
    u32 xy1;
    FxCommonPacketUv uv1;
    u32 xy2;
    FxCommonPacketUv uv2;
} FxCommonModelFt3Packet;

typedef struct FxCommonModelFt4Packet {
    FxCommonPacketTag tag;
    FxCommonPacketRgb rgb;
    u32 xy0;
    FxCommonPacketUv uv0;
    u32 xy1;
    FxCommonPacketUv uv1;
    u32 xy2;
    FxCommonPacketUv uv2;
    u32 xy3;
    u8 u3, v3;
    u16 pad;
} FxCommonModelFt4Packet;

typedef struct FxCommonModelGt3Packet {
    FxCommonPacketTag tag;
    FxCommonPacketRgb rgb0;
    u32 xy0;
    FxCommonPacketUv uv0;
    FxCommonPacketRgb rgb1;
    u32 xy1;
    FxCommonPacketUv uv1;
    FxCommonPacketRgb rgb2;
    u32 xy2;
    FxCommonPacketUv uv2;
} FxCommonModelGt3Packet;

typedef struct FxCommonModelGt4Packet {
    FxCommonPacketTag tag;
    FxCommonPacketRgb rgb0;
    u32 xy0;
    FxCommonPacketUv uv0;
    FxCommonPacketRgb rgb1;
    u32 xy1;
    FxCommonPacketUv uv1;
    FxCommonPacketRgb rgb2;
    u32 xy2;
    FxCommonPacketUv uv2;
    FxCommonPacketRgb rgb3;
    u32 xy3;
    u8 u3, v3;
    u16 pad;
} FxCommonModelGt4Packet;

#define FX_COMMON_SCRATCHPAD ((FxCommonRenderScratchpad *)0x1F800000)


#endif
