#include "common.h"
#include "pe1/gte.h"
#include "pe1/room_m089_trail.h"

/* Spiral trail: the history shift is written out field by field (retail
 * loads all fifteen halfwords before the first store), and the flash angle
 * is `timer * 128`, which keeps the signed lh that `timer << 7` loses. */

int func_801924F8(int mode, RoomM089SpiralTrail *trail, GteShortVector *anchor) {
    GteRotation rotation;
    RenderColor head = D_8018F1CC;
    RenderColor tail = D_8018F1D0;
    RenderMatrixSlot *matrixSlot;
    int radius;
    int intensity;
    int kind;
    int palette;

    switch (mode) {
    case 1:
        switch (trail->state) {
        case 0:
            trail->timer++;
            trail->points[5].x = trail->points[4].x;
            trail->points[5].y = trail->points[4].y;
            trail->points[5].z = trail->points[4].z;
            trail->points[4].x = trail->points[3].x;
            trail->points[4].y = trail->points[3].y;
            trail->points[4].z = trail->points[3].z;
            trail->points[3].x = trail->points[2].x;
            trail->points[3].y = trail->points[2].y;
            trail->points[3].z = trail->points[2].z;
            trail->points[2].x = trail->points[1].x;
            trail->points[2].y = trail->points[1].y;
            trail->points[2].z = trail->points[1].z;
            trail->points[1].x = trail->points[0].x;
            trail->points[1].y = trail->points[0].y;
            trail->points[1].z = trail->points[0].z;
            trail->points[0].x = anchor->x;
            trail->points[0].y = anchor->y;
            trail->points[0].z = anchor->z;
            trail->points[0].y -= trail->rise - trail->rise * trail->timer / 16;
            radius = trail->points[0].pad - trail->points[0].pad * trail->timer / 16;
            trail->points[0].x += func_80077DC4(trail->angle) * radius / 4096;
            trail->points[0].z += func_80077CF4(trail->angle) * radius / 4096;
            trail->angle += trail->points[1].pad;
            if (trail->timer >= 16) {
                trail->state = 1;
                trail->timer = 0;
            }
            break;
        case 1:
            trail->timer++;
            trail->points[0].x = anchor->x;
            trail->points[0].y = anchor->y;
            trail->points[0].z = anchor->z;
            if (trail->timer >= 8) return 1;
            break;
        default:
            return 0;
        }
        break;
    case 2:
        matrixSlot = &D_800BCFA4;
        gte_ldrotmatrix(matrixSlot->value);
        gte_ldtransmatrix(matrixSlot->value);
        switch (trail->state) {
        case 0:
            gte_ldrotmatrix(matrixSlot->value);
            gte_ldtransmatrix(matrixSlot->value);
            if (trail->points[5].x | trail->points[5].y | trail->points[5].z)
                func_800D2B58(&trail->points[4], &trail->points[5], &head, &tail, 0x19, 0, 1);
            if (trail->points[4].x | trail->points[4].y | trail->points[4].z)
                func_800D2B58(&trail->points[3], &trail->points[4], &head, &tail, 0x32, 0x19, 1);
            if (trail->points[3].x | trail->points[3].y | trail->points[3].z)
                func_800D2B58(&trail->points[2], &trail->points[3], &head, &tail, 0x4B, 0x32, 1);
            if (trail->points[2].x | trail->points[2].y | trail->points[2].z)
                func_800D2B58(&trail->points[1], &trail->points[2], &head, &tail, 0x64, 0x4B, 1);
            if (trail->points[1].x | trail->points[1].y | trail->points[1].z)
                func_800D2B58(&trail->points[0], &trail->points[1], &head, &tail, 0x80, 0x64, 1);
            break;
        case 1:
            rotation.x = 0x400;
            rotation.y = 0;
            rotation.z = trail->timer * 128;
            rotation.flags = 1;
            intensity = func_80077DC4(trail->timer << 7) / 32;
            kind = D_800F336C;
            palette = D_800E1204[kind];
            func_800CEE20(&trail->points[0], &rotation, 0x1000, 0x1000, 6,
                          (u16)func_80077AA4(0, (kind == 4 && D_800F3428) ? palette + 8 : palette + 4),
                          1, intensity, &head);
            break;
        default:
            return 0;
        }
        break;
    default:
        return 0;
    }
    return 0;
}
