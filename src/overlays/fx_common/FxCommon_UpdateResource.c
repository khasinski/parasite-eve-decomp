/* CC1_FLAGS: -fno-force-addr */
#include "fx_common.h"

s16 func_801958D4(s16 resourceId, u8 action)
{
    s16 result;
    s16 id;

    result = 0;
    id = resourceId;
    if ((u8)action == 0) {
        if (id != g_FxCommonResourceState.active) {
            func_8003746C(g_FxCommonResourceState.current.half);
            func_80038940(id, 0x80, 0x80, 0x80);
            func_800375E0(id, 3, &result);
            g_FxCommonResourceState.current.word = id;
        }
    }

    if ((u8)action == 1) {
        func_8003746C(g_FxCommonResourceState.current.half);
    }

    return (s16)resourceId;
}
