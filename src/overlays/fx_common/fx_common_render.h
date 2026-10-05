#ifndef FX_COMMON_RENDER_H
#define FX_COMMON_RENDER_H

#include "fx_common.h"

typedef GteShortVector FxCommonVector;
s32 func_80079384(const FxCommonVector *, const FxCommonVector *, const FxCommonVector *, u32 *,
                  u32 *, u32 *, s32 *, s32 *, u32 *);
s32 func_80079414(const FxCommonVector *, const FxCommonVector *, const FxCommonVector *,
                  const FxCommonVector *, u32 *, u32 *, u32 *, u32 *, s32 *, s32 *, u32 *);

struct FxCommonPolyResource {
    u16 counts[8];
    s32 offsets[8];
};

typedef union FxCommonUv {
    u16 packed;
    struct {
        u8 u;
        u8 v;
    } components;
} FxCommonUv;
typedef struct FxCommonRenderScratchpad {
    u32 screenCoordinates[4];
    s32 perspective;
    u32 transformFlags;
    s32 orderingDepth;
    s32 reserved1c;
    s32 normalClip;
    s32 depthBias;
    FxCommonVector *vertices[4];
    FxCommonVector midpoints[5];
    u32 midpointScreenCoordinates[5];
    FxCommonUv midpointUv[6];
} FxCommonRenderScratchpad;
typedef struct FxCommonFt4Packet {
    FxCommonPacketTag tag;
    u32 color;
    u32 xy0;
    u16 uv0;
    u16 clut;
    u32 xy1;
    u16 uv1;
    u16 tpage;
    u32 xy2;
    u16 uv2;
    u16 pad0;
    u32 xy3;
    u16 uv3;
    u16 pad1;
} FxCommonFt4Packet;

typedef struct FxCommonTextureHeader {
    u16 clut;
    FxCommonUv uv1;
    u16 tpage;
} FxCommonTextureHeader;
typedef struct FxCommonFt3Packet {
    FxCommonPacketTag tag;
    u32 color;
    u32 xy0;
    u16 uv0;
    u16 clut;
    u32 xy1;
    u16 uv1;
    u16 tpage;
    u32 xy2;
    u16 uv2;
    u16 pad;
} FxCommonFt3Packet;
typedef char FxCommonFt3PacketSizeCheck[(sizeof(FxCommonFt3Packet) == 32) ? 1 : -1];
typedef struct FxCommonTextureSet {
    FxCommonUv uv0;
    u16 clut;
    FxCommonUv uv1;
    u16 tpage;
    FxCommonUv uv2;
    FxCommonUv uv3;
} FxCommonTextureSet;
typedef struct FxCommonTexturedQuad {
    u32 color;
    FxCommonTextureSet textures[3];
    FxCommonVector vertices[4];
} FxCommonTexturedQuad;
typedef struct FxCommonTexturedTriangle {
    u32 color;
    FxCommonTextureSet textures[3];
    FxCommonVector vertices[3];
} FxCommonTexturedTriangle;

typedef struct FxCommonGt3Packet {
    FxCommonPacketTag tag;
    u32 color0, xy0, texture0, color1, xy1, texture1, color2, xy2, texture2;
} FxCommonGt3Packet;
typedef struct FxCommonGt4Packet {
    FxCommonPacketTag tag;
    u32 color0, xy0, texture0, color1, xy1, texture1, color2, xy2, texture2, color3, xy3;
    u16 uv3, pad;
} FxCommonGt4Packet;
typedef struct FxCommonF3Packet {
    FxCommonPacketTag tag;
    u32 color, xy0, xy1, xy2;
} FxCommonF3Packet;
typedef struct FxCommonF4Packet {
    FxCommonPacketTag tag;
    u32 color, xy0, xy1, xy2, xy3;
} FxCommonF4Packet;
typedef struct FxCommonG3Packet {
    FxCommonPacketTag tag;
    u32 color0, xy0, color1, xy1, color2, xy2;
} FxCommonG3Packet;
typedef struct FxCommonG4Packet {
    FxCommonPacketTag tag;
    u32 color0, xy0, color1, xy1, color2, xy2, color3, xy3;
} FxCommonG4Packet;
typedef struct FxCommonFlatTriangle {
    u32 color;
    FxCommonVector vertices[3];
} FxCommonFlatTriangle;
typedef struct FxCommonFlatQuad {
    u32 color;
    FxCommonVector vertices[4];
} FxCommonFlatQuad;
typedef struct FxCommonColoredTriangle {
    u32 colors[3];
    FxCommonVector vertices[3];
} FxCommonColoredTriangle;
typedef struct FxCommonColoredQuad {
    u32 colors[4];
    FxCommonVector vertices[4];
} FxCommonColoredQuad;
typedef struct FxCommonColoredTexturedTriangle {
    u32 colors[3];
    FxCommonTextureSet texture;
    u8 unknown18[12];
    FxCommonVector vertices[3];
} FxCommonColoredTexturedTriangle;
typedef struct FxCommonColoredTexturedQuad {
    u32 colors[4];
    FxCommonTextureSet texture;
    u8 unknown1c[16];
    FxCommonVector vertices[4];
} FxCommonColoredTexturedQuad;
typedef struct FxGt3CursorSlot {
    FxCommonColoredTexturedTriangle *cursor;
} __attribute__((aligned(8))) FxGt3CursorSlot;
typedef struct FxGt4CursorSlot {
    FxCommonColoredTexturedQuad *cursor;
} __attribute__((aligned(8))) FxGt4CursorSlot;

extern s32 D_801EA5E0;

#endif
