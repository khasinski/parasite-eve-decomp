#include "fx_common_motion.h"

/* Aim the effect camera from its position at its target: build the view
 * matrix, the look-at matrix and the billboard rotation used by sprites. */
void func_8018F05C(void)
{
    GteVector offset;
    GteVector unit;
    RoomSpriteMatrix view;
    RoomSpriteMatrix rotation;
    GteShortVector target;
    GteShortVector eye;
    GteVector up;

    view = D_8018EFF4;
    rotation = D_8018EFF4;
    offset.x = g_FxCommonCameraTarget.x - g_FxCommonCameraPosition.x;
    offset.y = g_FxCommonCameraTarget.y - g_FxCommonCameraPosition.y;
    offset.z = g_FxCommonCameraTarget.z - g_FxCommonCameraPosition.z;
    VectorNormal(&offset, &unit);
    view.t[0] = -g_FxCommonCameraPosition.x;
    view.t[1] = -g_FxCommonCameraPosition.y;
    view.t[2] = -g_FxCommonCameraPosition.z;
    D_8019BFC4.x = ratan2(unit.y, SquareRoot0(unit.x * unit.x + unit.z * unit.z));
    D_8019BFC4.y = ratan2(unit.z, unit.x) - 0x400;
    D_8019BFC4.z = 0;
    RotMatrix((RoomFxSeed8 *)&D_8019BFC4, &rotation);
    CompMatrix(&rotation, &view, &D_8019CC30);

    up.x = 0;
    up.y = -20000;
    up.z = 0;
    target.x = g_FxCommonCameraTarget.x;
    target.y = g_FxCommonCameraTarget.y;
    target.z = g_FxCommonCameraTarget.z;
    eye.x = g_FxCommonCameraPosition.x;
    eye.y = g_FxCommonCameraPosition.y;
    eye.z = g_FxCommonCameraPosition.z;
    func_8018F344((GteMatrix *)&D_8019CC30, &eye, &target, &up);

    D_8019BFC4.x = -ratan2(SquareRoot0(offset.x * offset.x + offset.z * offset.z),
                              -offset.y) + 0x400;
    D_8019BFC4.y = -ratan2(offset.z, offset.x) + 0x400;
    D_8019BFC4.z = 0;
    RotMatrixZYX(&D_8019BFC4, &D_8019CDF0);
    D_8019CDF0.t[0] = g_FxCommonCameraPosition.x;
    D_8019CDF0.t[1] = g_FxCommonCameraPosition.y;
    D_8019CDF0.t[2] = g_FxCommonCameraPosition.z;
    SetTransMatrix(&D_8019CC30);
    SetRotMatrix(&D_8019CC30);
}
