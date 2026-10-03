#include "pe1/room_ember_burst.h"

/* Fan sweep spark: circles the published anchor while it rises, dropping a
 * trail point now and then; it swells for 12 frames, holds for 12, then
 * fades while its radius grows. */
int func_80192718(int mode, RoomFanSweepSpark *spark) {
    GteShortVector position;
    GteRotation spin;
    RenderColor color = D_8018F1F0;
    GteShortVector *point;
    int size;
    int alpha;

    switch (mode) {
    case 1:
        spark->timer++;
        spark->y = D_800E27EC * 700 / 36 - 700 + D_801998F8.y;
        spark->x = D_801998F8.x + func_80077DC4((s16)spark->angle) * spark->radius / 4096;
        spark->z = D_801998F8.z + func_80077CF4((s16)spark->angle) * spark->radius / 4096;
        spark->angle += 0x80;
        if (!(func_80071A54() & 7)) {
            point = (GteShortVector *)func_800CE610(D_80199900);
            if (point) {
                point->x = spark->x;
                point->y = spark->y;
                point->z = spark->z;
            }
        }
        if (D_800E27EC < 0x24) break;
        return 1;
    case 2:
        switch (spark->state) {
        case 0:
            size = func_80077CF4(((s16)spark->timer << 10) / 12);
            alpha = 0x40;
            if ((s16)spark->timer < 12) break;
            spark->timer = 0;
            spark->state = 1;
            break;
        case 1:
            size = 0x1000;
            alpha = 0x40;
            if ((s16)spark->timer < 12) break;
            spark->timer = 0;
            spark->state = 2;
            break;
        default:
            size = 0x1000;
            alpha = func_80077DC4(((s16)spark->timer << 10) / 12) / 64;
            spark->radius += (s16)spark->timer * 4;
            break;
        }
        position.x = spark->x;
        position.y = spark->y;
        position.z = spark->z;
        size = size * 3 / 2;
        spin.x = 0;
        spin.y = 0;
        spin.z = D_800E27EC << 7;
        spin.flags = 0;
        {
            int kind = D_800F3368.palette;
            int palette = D_800E1204[kind];
            func_800CEE20(&position, &spin, size, size, 0x24,
                          GetClut(0, (kind == 4 && D_800F3428 != 0) ? palette + 9 : palette + 5),
                          1, alpha, &color);
        }
        break;
    }
    return 0;
}
