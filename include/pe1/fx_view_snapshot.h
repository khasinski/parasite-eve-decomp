#ifndef PE1_FX_VIEW_SNAPSHOT_H
#define PE1_FX_VIEW_SNAPSHOT_H

#include "common.h"

/* Eight-word copy of the active transform/view block in room and scene FX. */
typedef struct FxViewSnapshot {
    s32 w[8];
} FxViewSnapshot;

PE1_STATIC_ASSERT(sizeof(FxViewSnapshot) == 0x20,
                  fx_view_snapshot_size);

#endif /* PE1_FX_VIEW_SNAPSHOT_H */
