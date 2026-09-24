/* CC1_FLAGS: -fno-schedule-insns */
#include "fx_common.h"

void func_80195BC8(FxCommonShortVec3 *a, FxCommonShortVec3 *b, int bi, int ai)
{
    int x, y, z, bz, ax, ay, az, u, v, w, one;
    u16 bzCopy, ayCopy, azCopy;
    register int pinnedBx asm("$9");
    register int pinnedBy asm("$8");

    x = b->x;
    pinnedBx = D_8019C810;
    pinnedBy = D_8019C814;
    D_8019C08C = x - pinnedBx;
    y = b->y;
    bz = D_8019C818;
    D_8019C090 = y - pinnedBy;
    z = b->z;
    D_8019C0AC = pinnedBx;
    ax = D_8019C330;
    D_8019C0AE = pinnedBy;
    ay = D_8019C334;
    D_8019C09C = 0;
    D_8019C0A0 = 0;
    D_8019C0A4 = 0;
    D_8019C0B0 = bz;
    D_8019C094 = z - bz;
    /* These are separate halfword snapshots, after the signed delta reads. */
    D_8019C0B4 = *(volatile u16 *)&b->x;
    D_8019C0B6 = *(volatile u16 *)&b->y;
    bzCopy = *(volatile u16 *)&b->z;
    bi &= 255;
    D_8019C056 = bi;
    D_8019C0B8 = bzCopy;

    u = a->x;
    D_8019C05C = u - ax;
    v = a->y;
    az = D_8019C338;
    D_8019C060 = v - ay;
    one = 1;
    D_8019C054 = one << bi;
    D_8019C06C = 0;
    w = a->z;
    D_8019C064 = w - az;
    D_8019C070 = 0;
    D_8019C074 = 0;
    D_8019C07C = ax;
    D_8019C07E = ay;
    D_8019C080 = az;
    D_8019C084 = *(volatile u16 *)&a->x;
    ayCopy = *(volatile u16 *)&a->y;
    ai &= 255;
    D_8019C086 = ayCopy;
    azCopy = *(volatile u16 *)&a->z;
    D_8019C052 = ai;
    D_8019C050 = one << ai;
    D_8019C088 = azCopy;
}
