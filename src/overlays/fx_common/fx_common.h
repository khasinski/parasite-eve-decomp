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
    u8 pad0[0x5C];
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

#endif
