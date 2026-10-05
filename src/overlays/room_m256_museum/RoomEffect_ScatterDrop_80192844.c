#include "common.h"
#include "pe1/room_m256_drop.h"

/* Flashes the scene when the drop touches the player. */
#define ROOM_M256_DROP_FLASH(radius)                                          \
    if (func_800C6B90(drop, radius) && drop->hazard && D_800E2368->active) {  \
        RoomM256DropChannel *channel = D_800F32D0;                            \
        if ((channel->pool->node->flags & 0x3F000000) == 0x01000000) {        \
            D_8009D254->actor->flags |= 0x4000;                               \
            channel->pool->node->flags =                                      \
                (channel->pool->node->flags & 0xC0FFFFFF) | 0x19000000;       \
            channel->pool->node->flags |= 0x80000000;                         \
        }                                                                     \
    }

/* Scatter drop: falls and jitters, sheds three kinds of splash children,
 * flashes the scene on contact with the player and expires once it leaves
 * the walkable area; the splash children slow down and bounce. */
int func_80192844(int mode, RoomM256Drop *drop) {
    RoomM256DropRotation rotation = D_8018F1CC;
    RoomM256DropColor color = D_8018F1D4;
    RoomM256Drop *child;
    RoomM256DropMatrixSlot *slot;
    int width;
    int height;
    int fall;
    int bounce;

    switch (mode) {
    case 1:
        switch (drop->state) {
        case 0:
            drop->timer++;
            drop->x += drop->vx;
            drop->y += drop->vy;
            drop->z += drop->vz;
            drop->x += (func_80071A54() & 0x1F) - 0x10;
            drop->y += (func_80071A54() & 0x1F) - 0x10;
            drop->z += (func_80071A54() & 0x1F) - 0x10;
            if (drop->y >= D_800942EC.y - 0x10 || drop->timer >= 0x18) {
                drop->state = 1;
                drop->y = D_800942EC.y - 0x10;
                drop->vy = 0;
            }
            if (drop->timer & 1) {
                if ((func_80071A54() & 7) == 0) {
                    child = func_800CE610(D_800F33E0->pool);
                    if (child) {
                        child->x = drop->x;
                        child->y = drop->y;
                        child->z = drop->z;
                        child->vx = drop->vx / 2 + (func_80071A54() & 0x1F) - 0x10;
                        child->vy = drop->vy / 2 + (func_80071A54() & 0x1F) - 0x10;
                        child->vz = drop->vz / 2 + (func_80071A54() & 0x1F) - 0x10;
                        child->state = 2;
                        child->timer = 0;
                    }
                }
                if ((func_80071A54() & 7) == 1) {
                    child = func_800CE610(D_800F33E0->pool);
                    if (child) {
                        child->x = drop->x;
                        child->y = drop->y;
                        child->z = drop->z;
                        child->vx = (func_80071A54() & 0x1F) - 0x10;
                        child->vy = (func_80071A54() & 0x1F) - 0x10;
                        child->vz = (func_80071A54() & 0x1F) - 0x10;
                        child->state = 3;
                        child->timer = 0;
                    }
                }
                if ((func_80071A54() & 0xF) == 2) {
                    child = func_800CE610(D_800F33E0->pool);
                    if (child) {
                        child->x = drop->x;
                        child->y = drop->y;
                        child->z = drop->z;
                        child->vx = (func_80071A54() & 0x1F) - 0x10;
                        child->vy = (func_80071A54() & 0x1F) - 0x18;
                        child->vz = (func_80071A54() & 0x1F) - 0x10;
                        child->state = 4;
                        child->timer = 0;
                    }
                }
            }
            ROOM_M256_DROP_FLASH(0x19A);
            if (func_8001CAB0(drop->x << 16, drop->z << 16, D_8009D248, D_8009D1CC) == 0) return 1;
            if (drop->timer >= 0x20) return 1;
            break;
        case 1:
            drop->timer++;
            drop->x += drop->vx;
            drop->y += drop->vy;
            drop->z += drop->vz;
            drop->x += (func_80071A54() & 0x1F) - 0x10;
            drop->z += (func_80071A54() & 0x1F) - 0x10;
            ROOM_M256_DROP_FLASH(0x26C);
            if (func_8001CAB0(drop->x << 16, drop->z << 16, D_8009D248, D_8009D1CC) == 0) return 1;
            if (drop->timer >= 0x20) return 1;
            break;
        case 2:
            drop->timer++;
            drop->x += drop->vx;
            drop->y += drop->vy;
            drop->z += drop->vz;
            drop->vx = drop->vx * 127 / 128;
            drop->vz = drop->vz * 127 / 128;
            fall = (u16)drop->vy - 2;
            drop->vy = fall;
            if (drop->y >= D_800942EC.y) {
                bounce = -(s16)fall;
                drop->vy = bounce;
            }
            if (drop->timer >= 0x10) return 1;
            break;
        case 3:
            drop->timer++;
            drop->x += drop->vx;
            drop->y += drop->vy;
            drop->z += drop->vz;
            drop->vx = drop->vx * 511 / 512;
            drop->vz = drop->vz * 511 / 512;
            fall = (u16)drop->vy - 3;
            drop->vy = fall;
            if (drop->y >= D_800942EC.y) {
                bounce = -(s16)fall;
                drop->vy = bounce;
            }
            if (drop->timer >= 0x10) return 1;
            break;
        case 4:
            drop->timer++;
            drop->x += drop->vx;
            drop->y += drop->vy;
            drop->z += drop->vz;
            drop->vx = drop->vx * 31 / 32;
            drop->vz = drop->vz * 31 / 32;
            fall = (u16)drop->vy - 3;
            drop->vy = fall;
            if (drop->y >= D_800942EC.y) {
                bounce = -(s16)fall;
                drop->vy = bounce;
            }
            if (drop->timer >= 0x10) return 1;
            break;
        }
        break;
    case 2:
        slot = &D_800BCFA4;
        gte_ldrotmatrix(slot->value);
        gte_ldtransmatrix(slot->value);
        switch (drop->state) {
        case 0:
            width = (func_80077CF4(drop->timer << 5) + 0x1200) * 2;
            height = (func_80077CF4(drop->timer << 5) + 0xE00) * 2;
            {
                int kind = D_800F3368.palette;
                int palette = D_800E1204[kind];
                if (kind == 4 && D_800F3428) palette += 4;
                func_800CEE20(drop, 0, width, height,
                              (s16)D_800F3368.parameter02 * (drop->timer / 4),
                              GetClut(0, palette), 1, 0x72, 0);
            }
            if (func_80071A54() & 3) return 0;
            func_800D004C(drop, 0xFA, 0xFA, 5, 0, width, width, &color, 0,
                          func_80077DC4(drop->timer << 5) / 32, 3);
            return 0;
        case 1:
            width = (func_80077CF4(drop->timer << 5) + 0x1200) * 2;
            {
                int kind = D_800F3368.palette;
                int palette = D_800E1204[kind];
                if (kind == 4 && D_800F3428) palette += 4;
                func_800CEE20(drop, &rotation, width, width,
                              (s16)D_800F3368.parameter02 * (drop->timer / 4) + 0x20,
                              GetClut(0x10, palette), 1, 0x80, 0);
            }
            break;
        case 2:
            width = func_80077CF4(drop->timer << 6) / 2 + 0x1200;
            {
                int kind = D_800F3368.palette;
                int palette = D_800E1204[kind];
                if (kind == 4 && D_800F3428) palette += 4;
                func_800CEE20(drop, 0, width, width,
                              (s16)D_800F3368.parameter02 * (drop->timer / 2),
                              GetClut(0x10, palette), 1, 0x60, 0);
            }
            return 0;
        case 3:
            width = func_80077CF4(drop->timer << 6) / 2 + 0x1000;
            {
                int kind = D_800F3368.palette;
                int palette = D_800E1204[kind];
                if (kind == 4 && D_800F3428) palette += 4;
                func_800CEE20(drop, 0, width, width,
                              (s16)D_800F3368.parameter02 * (drop->timer / 2) + 0x40,
                              GetClut(0x20, palette), 2, 0x50, 0);
            }
            break;
        case 4:
            width = func_80077DC4(drop->timer << 6) / 32 + 0x40;
            D_800F3368.parameter00 = 0x20;
            D_800F3368.parameter02 = 2;
            D_800F3368.extent_x = 0x20;
            D_800F3368.extent_y = 0x20;
            func_800D1DEC(drop, &color, width, 1);
            break;
        }
        break;
    }
    return 0;
}
