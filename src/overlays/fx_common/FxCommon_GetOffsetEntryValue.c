#include "fx_common.h"

int func_8019959C(FxCommonOffsetTable *table, int index)
{
    FxCommonOffsetByte *base;
    FxCommonOffsetByte *cursor;

    base = (FxCommonOffsetByte *)table;
    cursor = base + index * sizeof(s32);
    cursor = base + *(s32 *)cursor;
    return ((FxCommonOffsetEntry *)cursor)->value;
}
