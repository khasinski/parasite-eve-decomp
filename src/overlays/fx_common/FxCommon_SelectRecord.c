#include "fx_common.h"

FxCommonRecord *func_80191754(void)
{
    int record = D_801E4A8A[D_8019CC50].record;

    D_8019CC50 = record;
    if (record == 201) {
        record = -1;
    }
    if (record == -1) {
        return 0;
    }
    return &D_801E4E00[record];
}
