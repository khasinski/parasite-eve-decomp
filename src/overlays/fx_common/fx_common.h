#ifndef FX_COMMON_H
#define FX_COMMON_H

#include "common.h"
#include "pe1/room_fx.h"

typedef struct FxCommonSelection {
    s16 record;
    s16 pad2;
} FxCommonSelection;

typedef struct FxCommonIndexLink {
    s16 previous;
    s16 next;
} FxCommonIndexLink;

typedef struct FxCommonRecord {
    u8 data[108];
} FxCommonRecord;

typedef struct FxCommonNode FxCommonNode;

struct FxCommonNode {
    s16 id;
    s16 type;
    union {
        void *payload;
        struct {
            s16 variant;
            s16 subtype;
            void *payload;
        } configured;
    } args;
    u8 pad0C[0x10];
    s32 state[4];
    u8 pad2C[0x06];
    s16 value32;
    s16 value34;
    s16 value36;
    void *data38;
    void *data3C;
    u8 pad40[0x1C];
    s16 bucket;
    u8 pad5E[2];
    FxCommonNode *parent;
    FxCommonNode *previous;
    FxCommonNode *next;
};

PE1_STATIC_ASSERT(sizeof(FxCommonNode) == sizeof(FxCommonRecord),
                  fx_common_node_record_size);

typedef struct FxCommonBucket {
    FxCommonNode *head;
    u8 pad4[104];
} FxCommonBucket;

typedef struct FxCommonBuffer {
    void *data;
    void *allocation;
} FxCommonBuffer;

typedef struct FxCommonBufferRegion {
    u8 data[0x15F90];
} FxCommonBufferRegion;

typedef struct FxCommonGradientQuad {
    u32 tag;
    u8 color0[4];
    s16 x0;
    s16 y0;
    u8 color1[4];
    s16 x1;
    s16 y1;
    u8 color2[4];
    s16 x2;
    s16 y2;
    u8 color3[4];
    s16 x3;
    s16 y3;
} FxCommonGradientQuad;

typedef struct FxCommonDrawModePacket {
    u32 tag;
    u32 command;
} FxCommonDrawModePacket;

typedef union FxCommonPacketCursor {
    FxCommonGradientQuad quad;
    struct {
        FxCommonDrawModePacket packet;
        u8 pad08[0x1C];
    } mode;
} FxCommonPacketCursor;

typedef union FxCommonAddress {
    void *pointer;
    u32 word;
} FxCommonAddress;

PE1_STATIC_ASSERT(sizeof(FxCommonGradientQuad) == 0x24,
                  fx_common_gradient_quad_size);
PE1_STATIC_ASSERT(sizeof(FxCommonDrawModePacket) == 8,
                  fx_common_draw_mode_packet_size);
PE1_STATIC_ASSERT(sizeof(FxCommonPacketCursor) == 0x24,
                  fx_common_packet_cursor_size);

typedef union FxCommonTransformSeed {
    RoomFxSeed8 room;
    s16 component[4];
} FxCommonTransformSeed;

typedef struct FxCommonTransformNode {
    u8 pad0[4];
    void *resource;              /* 0x04 */
    RoomSpriteMatrix matrix;     /* 0x08; translation Z doubles as lifetime */
    FxCommonTransformSeed seed;  /* 0x28 */
    u8 pad30[0x30];
    void *record;                /* 0x60 */
    void *previous;              /* 0x64 */
    void *next;                  /* 0x68 */
} FxCommonTransformNode;

PE1_STATIC_ASSERT(sizeof(FxCommonTransformSeed) == 8,
                  fx_common_transform_seed_size);
PE1_STATIC_ASSERT(sizeof(FxCommonTransformNode) == sizeof(FxCommonRecord),
                  fx_common_transform_node_record_size);

typedef struct FxCommonVec3 {
    s32 x;
    s32 y;
    s32 z;
} FxCommonVec3;

typedef struct FxCommonShortVec3 {
    s16 x;
    s16 y;
    s16 z;
} FxCommonShortVec3;

typedef struct FxCommonOffsetTable {
    s32 offsets[1];
} FxCommonOffsetTable;

typedef struct FxCommonOffsetEntry {
    u8 pad00[0x30];
    s32 value;
} FxCommonOffsetEntry;

typedef struct FxCommonOffsetByte {
    u8 value;
} FxCommonOffsetByte;

typedef struct FxCommonMotionVec {
    s32 x;
    s32 y;
    s32 z;
    s32 pad0C;
} FxCommonMotionVec;

typedef struct FxCommonMotionLane {
    s32 value;
    s32 pad04[3];
} FxCommonMotionLane;

typedef struct FxCommonMotionByte {
    u8 value;
} FxCommonMotionByte;

typedef struct FxCommonResourceState {
    s32 active;
    union {
        s16 half;
        s32 word;
    } current;
} FxCommonResourceState;

extern s16 D_8019C058;
extern FxCommonMotionVec D_8019CAA8[10];
extern FxCommonMotionLane D_8019CAB0[10];
extern FxCommonMotionByte D_801EA268[1];
extern FxCommonMotionByte D_801EA26C[1];
extern FxCommonMotionByte D_801EA270[1];
extern FxCommonMotionVec g_FxCommonMotionDeltas[10] __asm__("D_801EA268");
extern FxCommonResourceState g_FxCommonResourceState __asm__("D_8019C140");

extern s16 D_8019CC50;
extern FxCommonBucket D_8019CCBC[];
extern FxCommonSelection D_801E4A8A[];
extern FxCommonRecord D_801E4E00[];
extern s16 D_8019C830[];
extern s16 D_8019C9D0;
extern u16 D_8019CBC0;
extern FxCommonIndexLink D_801E4A88[];

extern u8 D_800B0DCE;
extern u8 D_800B0DCF;
extern s16 D_800B0DD0;
extern s16 D_800B0DD2;
extern FxCommonBufferRegion *D_800B0E4C;
extern u8 D_8019C1F8;
extern FxCommonBuffer *D_8019C9C0;
extern u8 D_8019C340;
extern RoomSpriteMatrix D_8019CC30;
extern RoomSpriteMatrix *D_8019BFF0;
extern u8 D_801D0260;
extern void *D_800BCFA4;
extern void *volatile D_800BCFA8;

void func_800868AC(int mode, int arg1);
void func_80077BC4(FxCommonGradientQuad *packet);
void func_80077B04(FxCommonGradientQuad *packet, int enabled);
void func_80077C84(FxCommonDrawModePacket *packet, int x, int y, int tpage);
void func_800752AC(void *allocation, int size);
void *func_8006EC6C(void *data, int count);
void func_8018F55C(int angle, int radius, void *allocation,
                   FxCommonMotionVec *vector, void *extra);
int func_8006DF50(void *resource, int arg1, int arg2, int arg3, int enabled);
void *func_80078A94(void);
void func_800787D4(RoomSpriteMatrix *left, RoomSpriteMatrix *right,
                   RoomSpriteMatrix *result);
void func_800794C4(RoomFxSeed8 *seed, RoomSpriteMatrix *matrix);
void func_80078E94(void *matrix);
void func_80078E04(void *matrix);
int func_80078B38(void);
void func_8006DFA8(FxCommonShortVec3 *input, int *outA, int *outB);
void func_800868F0(void *object, int index, int value);
void func_80086A28(void *object, int index, int value);
void func_8003746C(int resourceId);
void func_80038940(int resourceId, int red, int green, int blue);
void func_800375E0(int resourceId, int mode, s16 *result);
void func_801941A4(int value);
void func_80191678(s16 id);
void func_80191834(FxCommonNode *node);
void func_8019BF8C(void **buffer);
void func_80197BA0(void *context, void *resource);

#endif
