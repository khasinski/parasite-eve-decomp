#include "pe1/room_spark.h"
#include "pe1/render_prim.h"
#include "pe1/gte.h"
#ifndef ROOMEFFECT_BEAM_SPARK_FUNC
#error "ROOMEFFECT_BEAM_SPARK_FUNC must name the room entry point"
#endif
#ifndef ROOMEFFECT_BEAM_SPARK_CALLBACK
#error "ROOMEFFECT_BEAM_SPARK_CALLBACK must name the bouncing spark callback"
#endif
#ifndef ROOMEFFECT_BEAM_SPARK_ROTATION
#error "ROOMEFFECT_BEAM_SPARK_ROTATION must name the actor rotation seed"
#endif
#ifndef ROOMEFFECT_BEAM_SPARK_MODEL_ROTATION
#error "ROOMEFFECT_BEAM_SPARK_MODEL_ROTATION must name the model rotation seed"
#endif
#ifndef ROOMEFFECT_BEAM_SPARK_ASSET
#error "ROOMEFFECT_BEAM_SPARK_ASSET must name the model asset pointer"
#endif

/* Flat-colour GPU line packet with its tag split into address and length. */
typedef struct RoomBeamLinePacket {
    u8 address[3], length;
    u8 r, g, b, code;
    s16 x0, y0, x1, y1;
} RoomBeamLinePacket;

typedef struct RoomBeamSparkMatrix {
    s16 m[3][3];
    s16 reserved12;
    s32 t[3];                     /* 0x14 */
} RoomBeamSparkMatrix;

/* First object of the primary channel's pool: its packet flags and the
 * actor state byte it owns. */
typedef struct RoomBeamSparkObject {
    u32 flags;
    u8 reserved[0x14];
    u8 *status;                   /* 0x18 */
} RoomBeamSparkObject;

typedef struct RoomBeamSparkPool {
    RoomBeamSparkObject *object;
} RoomBeamSparkPool;

typedef struct RoomBeamSparkActor {
    u8 reserved[0x4C];
    u32 flags;                    /* 0x4C */
} RoomBeamSparkActor;

typedef struct RoomBeamSparkBattleEntity {
    RoomBeamSparkActor *actor;
} RoomBeamSparkBattleEntity;

extern GteRotation ROOMEFFECT_BEAM_SPARK_ROTATION;
extern GteRotation ROOMEFFECT_BEAM_SPARK_MODEL_ROTATION;
extern void *ROOMEFFECT_BEAM_SPARK_ASSET;
extern int ROOMEFFECT_BEAM_SPARK_CALLBACK(int mode, RoomBounceSpark *spark);

/* The asset library pointer read as a one-field record: the load then
 * stays below the target stores, as retail keeps it. */
typedef struct RoomBeamSparkAssetSlot {
    void *asset;
} RoomBeamSparkAssetSlot;

extern RoomBeamSparkAssetSlot D_800B0E64_slot __asm__("D_800B0E64");
extern RoomBeamSparkBattleEntity *D_8009D254;
extern void *D_8009D248;
extern u16 D_8009D1CC;
extern void *func_8006E498(void *base, u32 key);
extern void func_800C6D5C(void *asset, int x, int y);
extern void func_800783E4(void *from, void *to, int weightFrom, int weightTo,
                          void *out);
extern int func_8001CAB0(int x, int z, void *map, int flags);
extern int func_800C6B90(void *position, int radius);
extern s32 func_80077A64(s32, s32, s32, s32);
extern void func_800C6EC0(int tpage, int clut);
extern void func_800C6ED8(int);
extern void func_80079754(void *rotation, void *matrix);
extern void func_80078CC4(void *matrix, void *scale);
extern void func_800C6EF8(void *asset);
extern void func_800C6FA0(void *asset, int brightness);
extern void func_800C71E4(void *asset, void *matrix);
extern void func_800C6F4C(void *asset);

/* Controller that fires a beam from the actor to the target: it blends the
 * beam tip towards the target while spraying sparks, holds the beam while
 * a scaled model pulses at the target and hits anything in reach, then
 * fades the model out. */
int ROOMEFFECT_BEAM_SPARK_FUNC(int mode, RoomBeamSpark *fx,
                               RoomBeamSparkParams *params) {
    GteRotation rotation = ROOMEFFECT_BEAM_SPARK_ROTATION;
    GteRotation modelRotation = ROOMEFFECT_BEAM_SPARK_MODEL_ROTATION;
    GteShortVector actor;
    int depth;
    int scratch;
    RoomBeamLinePacket *line;
    RoomBounceSpark *child;
    RoomBeamSparkPool *pool;
    RoomBeamSparkObject *object;
    RoomSparkChannel *channel;
    RenderMatrixSlot *matrixSlot;
    u16 *tpages;
    u16 *index;
    int angle;
    int speed;
    int handle;
    int i;

    switch (mode) {
    case 0:
        fx->state = 0;
        fx->timer = 0;
        fx->scale = 0;
        fx->glow = 0;
        fx->countdown = 0;
        fx->tx = params->x;
        fx->ty = params->y;
        fx->tz = params->z;
        fx->ty = g_FrameCount16.count;
        ROOMEFFECT_BEAM_SPARK_ASSET = func_8006E498(D_800B0E64_slot.asset, 0xC5485704);
        func_800C6D5C(ROOMEFFECT_BEAM_SPARK_ASSET, 0, 0);
        handle = func_800D3FD8();
        func_800D3F64(0x58A, handle);
        if (D_800E2368->active) {
            pool = (RoomBeamSparkPool *)D_800F32D0->pool;
            if (pool) {
                object = pool->object;
                if (object) {
                    if (*object->status == 1) *object->status = 2;
                }
            }
        }
        return func_800CE560(D_800F33E0->pool, 0x14, 0x20, ROOMEFFECT_BEAM_SPARK_CALLBACK);
    case 1:
        func_800CE8F0(D_800F32D0->pool, 0x1F, &rotation, fx);
        func_800CE9D4(D_800F32D0->pool, 0, &actor);
        angle = -actor.y + 0x400;
        switch (fx->state) {
        case 0:
            fx->timer++;
            if ((s16)fx->timer == 1) {
                for (i = 0; i < 2; i++) {
                    child = func_800CE610_bounce(D_800F33E0->pool);
                    if (child) {
                        child->x = fx->x;
                        child->y = fx->y;
                        child->z = fx->z;
                        child->vx = rcos(angle) / 128;
                        child->vz = rsin(angle) / 128;
                        child->vy = 0;
                        child->state = i;
                        child->timer = 0;
                    }
                }
            }
            scratch = 0x1000 - rcos(((s16)fx->timer << 10) / 6);
            func_800783E4(fx, &fx->tx, 0x1000 - scratch, scratch, &fx->tipX);
            fx->brightness = 0x80 - ((s16)fx->timer << 5) / 6;
            if ((s16)fx->timer < 6) break;
            fx->state = 1;
            fx->timer = 0;
            for (i = 0; i < 8; i++) {
                child = func_800CE610_bounce(D_800F33E0->pool);
                if (child) {
                    child->x = fx->tx;
                    child->y = fx->ty;
                    child->z = fx->tz;
                    speed = (func_80071A54() & 0x1F) + 0x20;
                    child->vx = rcos(angle) * speed / 4096;
                    child->vz = rsin(angle) * speed / 4096;
                    child->vy = 0;
                    child->state = 0;
                    child->timer = 0;
                }
                angle += 0x200;
            }
            if (func_8001CAB0(fx->tx << 16, fx->tz << 16, D_8009D248, D_8009D1CC)) break;
            return 1;
        case 1:
            fx->timer++;
            fx->brightness = rcos(((s16)fx->timer << 10) / 6) / 128 + 0x40;
            fx->tipX = fx->tx;
            fx->tipY = fx->ty;
            fx->tipZ = fx->tz;
            fx->scale = rsin(((s16)fx->timer << 10) / 6) + 0x800;
            fx->glow = 0x80;
            if ((s16)fx->timer < 6) break;
            fx->state = 2;
            fx->timer = 0;
            fx->brightness = 0;
            for (i = 0; i < 0x16; i++) {
                child = func_800CE610_bounce(D_800F33E0->pool);
                if (child) {
                    scratch = func_80071A54() & 0xFFF;
                    func_800783E4(fx, &fx->tx, 0x1000 - scratch, scratch, child);
                    child->vy = 0;
                    child->ay = (func_80071A54() & 3) + 1;
                    child->state = 1;
                    child->timer = 0;
                }
            }
            break;
        case 2:
            fx->timer++;
            if (fx->scale > 0x1000) fx->scale -= 0x400;
            fx->glow = rsin(((s16)fx->timer << 14) / params->duration) / 128 + 0x80;
            if (fx->countdown != 0) fx->countdown--;
            if (func_800C6B90(&fx->tx, 0x258) && fx->countdown == 0) {
                if (D_800E2368->active) {
                    channel = D_800F32D0;
                    if ((((RoomBeamSparkPool *)channel->pool)->object->flags & 0x3F000000) == 0x01000000) {
                        D_8009D254->actor->flags |= 0x4000;
                        ((RoomBeamSparkPool *)channel->pool)->object->flags =
                            (((RoomBeamSparkPool *)channel->pool)->object->flags & 0xC0FFFFFF) | 0x19000000;
                        ((RoomBeamSparkPool *)channel->pool)->object->flags |= 0x80000000;
                    }
                }
                fx->countdown = params->interval;
            }
            if ((s16)fx->timer < params->duration) break;
            fx->state = 3;
            fx->timer = 0;
            break;
        case 3:
            fx->timer++;
            fx->scale = rcos((s16)fx->timer << 5);
            fx->glow = rcos((s16)fx->timer << 5) / 32;
            if ((s16)fx->timer >= 0x20) return 1;
            break;
        }
        break;
    case 2:
        matrixSlot = &D_800BCFA4;
        gte_ldrotmatrix(matrixSlot->value);
        gte_ldtransmatrix(matrixSlot->value);
        if (fx->brightness != 0) {
            line = (RoomBeamLinePacket *)(D_800B0E38.packets[D_8009CDDC] + D_8009CDD8);
            D_8009CDD8 += 0x10;
            gte_ldv3(fx, &fx->tipX, fx);
            gte_rtpt_padded();
            line->length = 3;
            line->code = 0x40;
            line->r = fx->brightness;
            line->g = fx->brightness;
            line->b = fx->brightness;
            gte_stszotz(&depth);
            if ((u32)(depth - 1) < 0xFFF) {
                gte_stsxy3(&line->x0, &line->x1, &scratch);
                func_800CF6F8(D_800B0E38.ordering[D_8009CDDC] + depth * 4, line, 1);
            }
        }
        D_800F3368.parameter00 = 0x40;
        D_800F3368.parameter02 = 4;
        D_800F3368.extent_x = 0x40;
        D_800F3368.extent_y = 0x40;
        index = &D_800E11EA;
        tpages = D_800E2850;
        {
            int tpage = tpages[*index];
            D_800F3368.palette = 3;
            D_800F3368.parameter06 = 0;
            D_800F3368.parameter0A = 5;
            D_800F3368.depth = 0;
            D_800F3368.tpage = tpage;
        }
        if ((s16)fx->glow != 0) {
            RoomBeamSparkMatrix matrix;
            GteVector scale;
            u16 clut;
            int kind;
            int palette;
            int page;
            matrixSlot = &D_800BCFA4;
            gte_ldrotmatrix(matrixSlot->value);
            gte_ldtransmatrix(matrixSlot->value);
            {
                int tpage = tpages[*index];
                D_800F3368.palette = 3;
                D_800F3368.parameter06 = 0;
                D_800F3368.tpage = tpage;
            }
            page = (tpages[*index] | func_80077A64(0, 1, 0, 0)) & 0xFFFF;
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            clut = func_80077AA4(0x40, palette);
            func_800C6EC0(page, clut);
            func_800C6ED8(1);
            func_80079754(&modelRotation, &matrix);
            matrix.t[0] = fx->tx;
            matrix.t[1] = fx->ty;
            matrix.t[2] = fx->tz;
            scale.x = fx->scale * 3 / 2;
            scale.y = fx->scale * 3 / 2;
            scale.z = fx->scale * 3 / 2;
            func_80078CC4(&matrix, &scale);
            func_800C6EF8(ROOMEFFECT_BEAM_SPARK_ASSET);
            func_800C6FA0(ROOMEFFECT_BEAM_SPARK_ASSET, (u16)((s16)fx->glow / 2));
            func_800C71E4(ROOMEFFECT_BEAM_SPARK_ASSET, &matrix);
            func_800C6F4C(ROOMEFFECT_BEAM_SPARK_ASSET);
        }
        D_800F3368.parameter00 = 0x20;
        D_800F3368.parameter02 = 2;
        D_800F3368.extent_x = 0x20;
        D_800F3368.extent_y = 0x20;
        {
            int tpage = D_800E2850[D_800E11EA];
            D_800F3368.palette = 3;
            D_800F3368.parameter06 = 0;
            D_800F3368.parameter0A = 0;
            D_800F3368.depth = 8;
            D_800F3368.tpage = tpage;
        }
        break;
    }
    return 0;
}

#undef ROOMEFFECT_BEAM_SPARK_FUNC
#undef ROOMEFFECT_BEAM_SPARK_CALLBACK
#undef ROOMEFFECT_BEAM_SPARK_ROTATION
#undef ROOMEFFECT_BEAM_SPARK_MODEL_ROTATION
#undef ROOMEFFECT_BEAM_SPARK_ASSET
