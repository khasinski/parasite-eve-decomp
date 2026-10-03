#include "pe1/room_burst_spark.h"
#include "pe1/gte.h"
#ifndef ROOMEFFECT_BURST_SPARK_FUNC
#error "ROOMEFFECT_BURST_SPARK_FUNC must name the particle callback"
#endif
#ifndef ROOMEFFECT_BURST_SPARK_GLOW
#error "ROOMEFFECT_BURST_SPARK_GLOW must name the bright sprite colour"
#endif
#ifndef ROOMEFFECT_BURST_SPARK_SHADE
#error "ROOMEFFECT_BURST_SPARK_SHADE must name the dim sprite colour"
#endif

extern RenderColor ROOMEFFECT_BURST_SPARK_GLOW;
extern RenderColor ROOMEFFECT_BURST_SPARK_SHADE;

/* Particle callback of the burst spawner. State 0 flies along its heading,
 * drops a trail spark every other frame and bursts into six debris sparks
 * when it reaches the actor, the floor or a wall; trail (1) and debris (3)
 * sparks drift with damping and bounce off the floor; states 2 and 4 are
 * timed fades. Mode 2 draws each state as scaled sprites. */
int ROOMEFFECT_BURST_SPARK_FUNC(int mode, RoomBurstSpark *spark, s16 *distance) {
    GteShortVector target;
    RenderColor glow = ROOMEFFECT_BURST_SPARK_GLOW;
    RenderColor shade = ROOMEFFECT_BURST_SPARK_SHADE;
    GteShortVector ground;
    GteShortVector flat;
    RoomBurstSpark *child;
    char *pool;
    int i;
    int kind;
    int palette;
    u16 clut;
    int opacity;
    int scale;
    int fall;
    int bounce;
    int one;

    switch (mode) {
    case 1:
        switch (spark->state) {
        case 0:
            spark->timer++;
            func_800CFB7C((GteShortVector *)&spark->vx, *distance, &target);
            spark->x += target.x;
            spark->y += target.y;
            spark->z += target.z;
            if (!(spark->timer & 1)) {
                pool = D_800F33E0->pool;
                child = (RoomBurstSpark *)func_800CE610(pool);
                if (child) {
                    child->x = spark->x;
                    child->y = spark->y;
                    child->z = spark->z;
                    child->vx = func_80071A54() % 32 - 16;
                    child->vy = func_80071A54() % 32 - 16;
                    child->vz = func_80071A54() % 32 - 16;
                    child->state = 1;
                    child->timer = 0;
                }
            }
            if (func_800C6B90(spark, 50) && D_800942EC.height - 0x202 < spark->y) {
                if (D_800E2368->active) {
                    RoomBurstSparkChannel *channel = D_800F32D0;
                    u32 *entry;
                    if ((**(u32 **)channel->pool & 0x3F000000) == 0x01000000) {
                        D_8009D254->actor->flags |= 0x4000;
                        entry = *(u32 **)channel->pool;
                        *entry = (*entry & 0xC0FFFFFF) | 0x19000000;
                        entry = *(u32 **)channel->pool;
                        *entry |= 0x80000000;
                    }
                }
                spark->state = 2;
                spark->timer = 0;
                for (i = 0; i < 6; i++) {
                    pool = D_800F33E0->pool;
                    child = (RoomBurstSpark *)func_800CE610(pool);
                    if (child) {
                        child->x = spark->x;
                        child->y = spark->y;
                        child->z = spark->z;
                        child->vx = func_80071A54() % 40 - 20;
                        child->vy = func_80071A54() % 40 - 20;
                        child->vz = func_80071A54() % 40 - 20;
                        child->state = 3;
                        child->timer = 0;
                    }
                }
            }
            if (spark->y >= D_800942EC.height) {
                spark->state = 2;
                spark->timer = 0;
                for (i = 0; i < 6; i++) {
                    pool = D_800F33E0->pool;
                    child = (RoomBurstSpark *)func_800CE610(pool);
                    if (child) {
                        child->x = spark->x;
                        child->y = spark->y;
                        child->z = spark->z;
                        child->vx = func_80071A54() % 40 - 20;
                        child->vy = func_80071A54() % 40 - 20;
                        child->vz = func_80071A54() % 40 - 20;
                        child->state = 3;
                        child->timer = 0;
                    }
                }
            }
            if (func_8001CAB0(spark->x << 16, spark->z << 16, D_8009D248,
                              D_8009D1CC) == 0) {
                spark->state = 2;
                spark->timer = 0;
            }
            break;
        case 1:
            spark->timer++;
            spark->x += spark->vx;
            spark->y += spark->vy;
            spark->z += spark->vz;
            spark->vx = spark->vx * 63 / 64;
            spark->vz = spark->vz * 63 / 64;
            fall = (u16)spark->vy - 1;
            spark->vy = fall;
            if (spark->y >= D_800942EC.height) {
                bounce = -(s16)fall;
                spark->vy = bounce;
            }
            if ((s16)spark->timer < 6) break;
            return 1;
        case 3:
            spark->timer++;
            spark->x += spark->vx;
            spark->y += spark->vy;
            spark->z += spark->vz;
            spark->vx = spark->vx * 63 / 64;
            spark->vz = spark->vz * 63 / 64;
            fall = (u16)spark->vy + 1;
            spark->vy = fall;
            if (spark->y >= D_800942EC.height) {
                bounce = -(s16)fall;
                spark->vy = bounce;
            }
            if ((s16)spark->timer < 12) break;
            return 1;
        case 2:
        case 4:
            spark->timer++;
            if ((s16)spark->timer < 16) break;
            return 1;
        }
        break;
    case 2:
        gte_ldrotmatrix(D_800BCFA4.value);
        gte_ldtransmatrix(D_800BCFA4.value);
        switch (spark->state) {
        case 0:
            opacity = 0x80 - ((spark->timer & 1) << 6);
            target.x = 0;
            target.y = 0;
            target.z = (s16)spark->timer * 128;
            target.pad = 0;
            /* Every draw fills the parameter block and then reads the
             * palette back through it: cse keeps the block address in s0,
             * so parameter00 is stored through it and the later reads are
             * addressed off it, while the other stores fold to absolute
             * addresses as in retail. */
            {
                int kind;
                int palette;

                D_800F3368.parameter00 = 0x10;
                D_800F3368.parameter02 = 1;
                D_800F3368.extent_x = 0x10;
                D_800F3368.extent_y = 0x10;
                D_800F3368.tpage = D_800E2850[D_800E11E8.index];
                D_800F3368.palette = 2;
                D_800F3368.parameter06 = 0;
                kind = D_800F3368.palette;
                palette = D_800E1204[kind];
                if (kind == 4 && D_800F3428 != 0) palette += 4;
                clut = func_80077AA4(0x20, palette);
            }
            func_800CEE20((GteShortVector *)spark, (GteRotation *)&target,
                          0x1000, 0x1000, 0xD8, clut, 1, opacity, &glow);
            {
                u16 kind = D_800F3368.palette;
                int palette = D_800E1204[kind];
                if (kind == 4 && D_800F3428 != 0) palette += 4;
                clut = func_80077AA4(0x20, palette);
            }
            func_800CEE20((GteShortVector *)spark, (GteRotation *)&target,
                          0x2000, 0x2000,
                          (s16)D_800F3368.parameter02 + 0xD8, clut, 1,
                          opacity / 2, &shade);
            flat.x = 0x400;
            flat.y = 0;
            flat.z = 0;
            flat.pad = 1;
            ground.x = spark->x;
            ground.z = spark->z;
            ground.y = D_800942EC.height;
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            clut = func_80077AA4(0x20, palette);
            func_800CEE20(&ground, (GteRotation *)&flat, 0x1000, 0x1000,
                          (s16)D_800F3368.parameter02 + 0xD8, clut, 1,
                          opacity, &shade);
            break;
        case 1: {
            u16 clut;
            int kind;
            int palette;

            opacity = func_80077CF4((s16)spark->timer << 7) / 64 + 0x40;
            D_800F3368.parameter00 = 0x10;
            D_800F3368.parameter02 = 1;
            D_800F3368.extent_x = 0x10;
            D_800F3368.extent_y = 0x10;
            D_800F3368.tpage = D_800E2850[D_800E11E8.index];
            D_800F3368.palette = 2;
            D_800F3368.parameter06 = 0;
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            clut = func_80077AA4(0x20, palette);
            func_800CEE20((GteShortVector *)spark, 0, 0x1000, 0x1000,
                          (s16)D_800F3368.parameter02 * (s16)spark->timer + 0xE0,
                          clut, 1, opacity, &shade);
            break;
        }
        case 2:
            opacity = func_80077DC4((s16)spark->timer << 6) / 32;
            if (spark->timer & 1) opacity = opacity * 2 / 3;
            D_800F3368.parameter00 = 0x20;
            D_800F3368.parameter02 = 2;
            D_800F3368.extent_x = 0x20;
            D_800F3368.extent_y = 0x20;
            D_800F3368.tpage = D_800E2850[D_800E11EC.index];
            D_800F3368.palette = 4;
            D_800F3368.parameter06 = 0;
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            clut = func_80077AA4(0, palette);
            one = 1;
            func_800CEE20((GteShortVector *)spark, 0, 0x1800, 0x1800,
                          (s16)D_800F3368.parameter02 *
                              ((s16)spark->timer / 2) + 0xC0,
                          clut, one, opacity, &glow);
            scale = func_80077CF4((s16)spark->timer << 6) + 0x800;
            func_800D004C((GteShortVector *)spark, 0x12C, 0x12C, 5, 0,
                          scale, scale, &shade, 0, opacity / 2, one);
            return 0;
        case 3: {
            u16 clut;
            int kind;
            int palette;
            int step;

            opacity = func_80077DC4(((s16)spark->timer << 10) / 12) / 32;
            D_800F3368.parameter00 = 0x10;
            D_800F3368.parameter02 = 1;
            D_800F3368.extent_x = 0x10;
            D_800F3368.extent_y = 0x10;
            D_800F3368.tpage = D_800E2850[D_800E11E8.index];
            D_800F3368.palette = 2;
            D_800F3368.parameter06 = 0;
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            clut = func_80077AA4(0x20, palette);
            step = (s16)spark->timer * 2 / 3;
            func_800CEE20((GteShortVector *)spark, 0, 0x1000, 0x1000,
                          (s16)D_800F3368.parameter02 * step + 0xC8, clut, 1,
                          opacity, &shade);
            break;
        }
        case 4: {
            u16 clut;
            int kind;
            int palette;

            scale = func_80077CF4((s16)spark->timer << 6) + 0x1000;
            opacity = func_80077DC4((s16)spark->timer << 6) / 32;
            spark->vw = 1;
            spark->vz = (s16)spark->timer * 24;
            D_800F3368.parameter00 = 0x10;
            D_800F3368.parameter02 = 1;
            D_800F3368.extent_x = 0x10;
            D_800F3368.extent_y = 0x10;
            D_800F3368.tpage = D_800E2850[D_800E11E8.index];
            D_800F3368.palette = 2;
            D_800F3368.parameter06 = 0;
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            clut = func_80077AA4(0x90, palette);
            func_800CEE20((GteShortVector *)spark,
                          (GteRotation *)&spark->vx, scale, scale,
                          (s16)D_800F3368.parameter02 * 2 + 0xFD, clut, 1,
                          opacity, 0);
            break;
        }
        }
        break;
    }
    return 0;
}

#undef ROOMEFFECT_BURST_SPARK_FUNC
#undef ROOMEFFECT_BURST_SPARK_GLOW
#undef ROOMEFFECT_BURST_SPARK_SHADE
