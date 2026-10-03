#ifndef ROOM_FAN_SWEEP_SPARK_NAME
#define ROOM_FAN_SWEEP_SPARK_NAME func_80192718
#define ROOM_FAN_SWEEP_SPARK_COLOR D_8018F1F0
#define ROOM_FAN_SWEEP_SPARK_ANCHOR D_801998F8
#define ROOM_FAN_SWEEP_SPARK_TRAIL D_80199900
#define ROOM_FAN_SWEEP_SPARK_COS func_80077DC4
#define ROOM_FAN_SWEEP_SPARK_SIN func_80077CF4
#define ROOM_FAN_SWEEP_SPARK_RAND func_80071A54
#define ROOM_FAN_SWEEP_SPARK_PALETTE_LIT 9
#define ROOM_FAN_SWEEP_SPARK_PALETTE_BASE 5
#endif

/* Fan sweep spark: circles the published anchor while it rises, dropping a
 * trail point now and then; it swells for 12 frames, holds for 12, then
 * fades while its radius grows. */
int ROOM_FAN_SWEEP_SPARK_NAME(int mode, RoomFanSweepSpark *spark) {
    GteShortVector position;
    GteRotation spin;
    RenderColor color = ROOM_FAN_SWEEP_SPARK_COLOR;
    GteShortVector *point;
    int size;
    int alpha;

    switch (mode) {
    case 1:
        spark->timer++;
        spark->y = D_800E27EC * 700 / 36 - 700 + ROOM_FAN_SWEEP_SPARK_ANCHOR.y;
        spark->x = ROOM_FAN_SWEEP_SPARK_ANCHOR.x + ROOM_FAN_SWEEP_SPARK_COS((s16)spark->angle) * spark->radius / 4096;
        spark->z = ROOM_FAN_SWEEP_SPARK_ANCHOR.z + ROOM_FAN_SWEEP_SPARK_SIN((s16)spark->angle) * spark->radius / 4096;
        spark->angle += 0x80;
        if (!(ROOM_FAN_SWEEP_SPARK_RAND() & 7)) {
            point = (GteShortVector *)func_800CE610(ROOM_FAN_SWEEP_SPARK_TRAIL);
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
            size = ROOM_FAN_SWEEP_SPARK_SIN(((s16)spark->timer << 10) / 12);
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
            alpha = ROOM_FAN_SWEEP_SPARK_COS(((s16)spark->timer << 10) / 12) / 64;
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
                          GetClut(0, (kind == 4 && D_800F3428 != 0) ? palette + ROOM_FAN_SWEEP_SPARK_PALETTE_LIT : palette + ROOM_FAN_SWEEP_SPARK_PALETTE_BASE),
                          1, alpha, &color);
        }
        break;
    }
    return 0;
}
