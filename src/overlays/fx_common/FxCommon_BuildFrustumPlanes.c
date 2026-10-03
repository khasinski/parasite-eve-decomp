#include "fx_common_motion.h"

/* Rotate the four frustum corners by the camera matrix and rebuild the
 * four side planes through the eye: normal, distance, the signed distance
 * of the next corner and the normal length. */
void func_8018F92C(FxCommonVec3 *eye)
{
    GteVector corner0;
    GteVector corner1;
    GteVector corner2;
    GteVector corner3;
    GteVector a;
    GteVector b;
    s32 flag;

    func_80078A94();
    func_80078E94(&D_8019CDF0);
    func_80078E04(&D_8019CDF0);
    func_800792D4(&D_8019BFD0, &corner0, &flag);
    func_800792D4(&D_8019BFD8, &corner1, &flag);
    func_800792D4(&D_8019BFE0, &corner2, &flag);
    func_800792D4(&D_8019BFE8, &corner3, &flag);

    a.x = corner2.x - eye->x;
    a.y = corner2.y - eye->y;
    a.z = corner2.z - eye->z;
    b.x = corner0.x - eye->x;
    b.y = corner0.y - eye->y;
    b.z = corner0.z - eye->z;
    func_800791D0(&a, &b, &D_8019CBB0);

    a.x = corner3.x - eye->x;
    a.y = corner3.y - eye->y;
    a.z = corner3.z - eye->z;
    b.x = corner1.x - eye->x;
    b.y = corner1.y - eye->y;
    b.z = corner1.z - eye->z;
    func_800791D0(&a, &b, &D_8019CBD0);

    a.x = corner3.x - eye->x;
    a.y = corner3.y - eye->y;
    a.z = corner3.z - eye->z;
    b.x = corner2.x - eye->x;
    b.y = corner2.y - eye->y;
    b.z = corner2.z - eye->z;
    func_800791D0(&a, &b, &D_8019CBF0);

    a.x = corner1.x - eye->x;
    a.y = corner1.y - eye->y;
    a.z = corner1.z - eye->z;
    b.x = corner0.x - eye->x;
    b.y = corner0.y - eye->y;
    b.z = corner0.z - eye->z;
    func_800791D0(&a, &b, &D_8019CB50);

    D_8019CB48 = -(D_8019CBB0.x * corner2.x) - D_8019CBB0.y * corner2.y
               - D_8019CBB0.z * corner2.z;
    D_8019CB4C = -(D_8019CBD0.x * corner3.x) - D_8019CBD0.y * corner3.y
               - D_8019CBD0.z * corner3.z;
    D_8019CBA8 = -(D_8019CBF0.x * corner2.x) - D_8019CBF0.y * corner2.y
               - D_8019CBF0.z * corner2.z;
    D_8019CA90 = -(D_8019CB50.x * corner1.x) - D_8019CB50.y * corner1.y
               - D_8019CB50.z * corner1.z;
    D_8019CC04 = D_8019CBB0.x * corner1.x + D_8019CBB0.y * corner1.y
               + D_8019CBB0.z * corner1.z + D_8019CB48;
    D_8019CC0C = D_8019CBD0.x * corner0.x + D_8019CBD0.y * corner0.y
               + D_8019CBD0.z * corner0.z + D_8019CB4C;
    D_8019CC10 = D_8019CBF0.x * corner1.x + D_8019CBF0.y * corner1.y
               + D_8019CBF0.z * corner1.z + D_8019CBA8;
    D_8019CBC4 = D_8019CB50.x * corner2.x + D_8019CB50.y * corner2.y
               + D_8019CB50.z * corner2.z + D_8019CA90;

    D_8019CBC8 = func_80078004(D_8019CBB0.x * D_8019CBB0.x
                               + D_8019CBB0.y * D_8019CBB0.y
                               + D_8019CBB0.z * D_8019CBB0.z);
    D_8019CC00 = func_80078004(D_8019CBD0.x * D_8019CBD0.x
                               + D_8019CBD0.y * D_8019CBD0.y
                               + D_8019CBD0.z * D_8019CBD0.z);
    D_8019CC08 = func_80078004(D_8019CBF0.x * D_8019CBF0.x
                               + D_8019CBF0.y * D_8019CBF0.y
                               + D_8019CBF0.z * D_8019CBF0.z);
    D_8019CBAC = func_80078004(D_8019CB50.x * D_8019CB50.x
                               + D_8019CB50.y * D_8019CB50.y
                               + D_8019CB50.z * D_8019CB50.z);
    func_80078B38();
}
