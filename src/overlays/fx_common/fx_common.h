#ifndef FX_COMMON_H
#define FX_COMMON_H

#include "common.h"

typedef struct FxCommonSelection {
    s16 record;
    s16 pad2;
} FxCommonSelection;

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

extern s16 D_8019CC50;
extern FxCommonBucket D_8019CCBC[];
extern FxCommonSelection D_801E4A8A[];
extern FxCommonRecord D_801E4E00[];

extern u8 D_800B0DCE;
extern u8 D_800B0DCF;
extern s16 D_800B0DD0;
extern s16 D_800B0DD2;

void func_800868AC(int mode, int arg1);
void func_80191678(int id);
void func_80191834(FxCommonNode *node);

#endif
