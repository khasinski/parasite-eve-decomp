#ifndef FX_COMMON_H
#define FX_COMMON_H

#include "common.h"

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
    u8 pad2[0x5A];
    s16 bucket;
    u8 pad5E[2];
    FxCommonNode *parent;
    FxCommonNode *previous;
    FxCommonNode *next;
};

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

typedef struct FxCommonTransformNode {
    u8 pad0[8];
    s16 matrix[10];               /* 0x08 */
    s32 translation[3];          /* 0x1C; Z doubles as lifetime */
    s16 extra[3];                /* 0x28 */
    u8 pad2E[0x32];
    void *record;                /* 0x60 */
    void *previous;              /* 0x64 */
    void *next;                  /* 0x68 */
} FxCommonTransformNode;

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

void func_800868AC(int mode, int arg1);
void func_800752AC(void *allocation, int size);
void func_80191678(s16 id);
void func_80191834(FxCommonNode *node);
void func_8019BF8C(void **buffer);

#endif
