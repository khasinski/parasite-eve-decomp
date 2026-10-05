#include "common.h"
#include "pe1/room_m404_drop.h"

/* Flashes the scene with the given code when the drop touches the player. */
#define ROOM_M404_DROP_FLASH(code)                                            \
    {                                                                         \
        RoomM404DropChannel *channel = D_800F32D0;                            \
        if ((channel->pool->node->flags & 0x3F000000) == 0x01000000) {        \
            D_8009D254->actor->flags |= 0x4000;                               \
            channel->pool->node->flags =                                      \
                (channel->pool->node->flags & 0xC0FFFFFF) | (code);           \
            channel->pool->node->flags |= 0x80000000;                         \
        }                                                                     \
    }

/* Sinking drop: sinks and jitters until it spreads on the floor, sheds
 * splash children, lingers as a pool that spits droplets and expires once
 * it leaves the walkable area; the splashes slow down and bounce. */
int func_80192540(int mode, RoomM404Drop *drop) {
    RoomM404DropRotation rotation = D_8018F1CC;
    RoomM404DropColor color = D_8018F1D4;
    RoomM404DropColor glow = D_8018F1D8;
    RoomM404DropColor fadeColor;
    RoomM404Drop *child;
    int size;
    int alpha;
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
            drop->x += (func_80071A54() & 0xF) - 8;
            drop->z += (func_80071A54() & 0xF) - 8;
            drop->y += 0x40;
            if (drop->y >= D_800942EC.y - 0x10) {
                drop->state = 1;
                drop->y = D_800942EC.y - 0x10;
                drop->vy = 0;
                drop->vx *= 2;
                drop->vz *= 2;
            }
            if (drop->timer & 1) {
                child = func_800CE610(D_800F33E0->pool);
                if (child) {
                    child->x = drop->x;
                    child->y = drop->y;
                    child->z = drop->z;
                    child->vx = (func_80071A54() & 0x1F) - 0x10;
                    child->vy = 0;
                    child->vz = (func_80071A54() & 0x1F) - 0x10;
                    child->state = 2;
                    child->timer = 0;
                }
            }
            if (func_800C6B90(drop, 0x6E) && drop->y >= D_800942EC.y - 0x202 &&
                D_800E2368->active) {
                ROOM_M404_DROP_FLASH(0x3D000000);
            }
            if (func_8001CAB0(drop->x << 16, drop->z << 16, D_8009D248, D_8009D1CC) == 0) return 1;
            break;
        case 1:
            drop->timer++;
            drop->x += drop->vx;
            drop->y += drop->vy;
            drop->z += drop->vz;
            drop->x += (func_80071A54() & 0x1F) - 0x10;
            drop->z += (func_80071A54() & 0x1F) - 0x10;
            if (drop->timer & 1) {
                child = func_800CE610(D_800F33E0->pool);
                if (child) {
                    child->x = drop->x;
                    child->y = drop->y;
                    child->z = drop->z;
                    child->vx = (func_80071A54() & 0x1F) - 0x10;
                    child->vy = 0;
                    child->vz = (func_80071A54() & 0x1F) - 0x10;
                    child->state = 2;
                    child->timer = 0;
                }
            }
            if (func_800C6B90(drop, 0x6E) && D_800E2368->active) {
                ROOM_M404_DROP_FLASH(0x19000000);
            }
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
            fall = (u16)drop->vy - 1;
            drop->vy = fall;
            if (drop->y >= D_800942EC.y) {
                bounce = -(s16)fall;
                drop->vy = bounce;
            }
            if (drop->timer < 0x10) return 0;
            drop->state = 4;
            drop->timer = 0;
            drop->y = D_800942EC.y;
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
            if (drop->timer >= 0xC) return 1;
            break;
        case 4:
            drop->timer++;
            if (func_800C6B90(drop, 0x6E) && D_800E2368->active) {
                ROOM_M404_DROP_FLASH(0x19000000);
            }
            if ((drop->timer & 1) && (func_80071A54() & 0xF) == 0) {
                child = func_800CE610(D_800F33E0->pool);
                if (child) {
                    child->x = drop->x;
                    child->y = drop->y;
                    child->z = drop->z;
                    child->x += (func_80071A54() & 0x7F) - 0x40;
                    child->y -= func_80071A54() & 0x3F;
                    child->z += (func_80071A54() & 0x7F) - 0x40;
                    child->vx = (func_80071A54() & 0x1F) - 0x10;
                    child->vy = -(func_80071A54() & 7);
                    child->vz = (func_80071A54() & 0x1F) - 0x10;
                    child->state = 3;
                    child->timer = 0;
                }
            }
            if (drop->timer >= 0x2E) return 1;
            break;
        }
        break;
    case 2:
        {
            RoomM404DropMatrixSlot *slot = &D_800BCFA4;
            gte_ldrotmatrix(slot->value);
            gte_ldtransmatrix(slot->value);
        }
        switch (drop->state) {
        case 0:
        case 1:
            size = func_80077CF4(D_800E27EC << 9) / 4 + 0x1000;
            alpha = (D_800E27EC & 1) * 48 + 0x80;
            {
                int kind = D_800F3368.palette;
                int palette = D_800E1204[kind];
                if (kind == 4 && D_800F3428) palette += 4;
                func_800CEE20(drop, 0, size, size, 0x20, GetClut(0x10, palette), 1, alpha,
                              &glow);
            }
            if (func_80071A54() & 3) return 0;
            {
                RoomM404DropMatrixSlot *slot = &D_800BCFA4;
                gte_ldrotmatrix(slot->value);
                gte_ldtransmatrix(slot->value);
            }
            func_800D004C(drop, 0xC8, 0xC8, 5, 0, size, size, &glow, 0, alpha, 1);
            break;
        case 2:
            func_800CF3AC(D_80193EFC, &fadeColor, (drop->timer << 4) / 12);
            {
                int kind = D_800F3368.palette;
                int palette = D_800E1204[kind];
                if (kind == 4 && D_800F3428) palette += 4;
                func_800CEE20(drop, 0, 0x1000, 0x1000,
                              (s16)D_800F3368.parameter02 * (drop->timer / 2) + 0x20,
                              GetClut(0x10, palette), 1, 0x80, &fadeColor);
            }
            return 0;
        case 3:
            size = func_80077CF4(drop->timer << 6) * 4 / 9 + 0x1000;
            {
                int kind = D_800F3368.palette;
                int palette = D_800E1204[kind];
                if (kind == 4 && D_800F3428) palette += 4;
                func_800CEE20(drop, 0, size, size,
                              (s16)D_800F3368.parameter02 * (drop->timer / 2),
                              GetClut(0, palette), 2, 0x20, 0);
            }
            return 0;
        case 4:
            alpha = func_80077CF4((drop->timer << 11) / 46) / 32;
            size = drop->timer & 0xF;
            if (size >= 8) size += 0x18;
            D_800F3368.extent_x = D_800F3368.parameter00;
            D_800F3368.extent_y = D_800F3368.parameter00 * 2;
            {
                int kind = D_800F3368.palette;
                int palette = D_800E1204[kind];
                if (kind == 4 && D_800F3428) palette += 4;
                func_800CEE20(drop, 0, 0x1800, 0x1000,
                              (s16)D_800F3368.parameter02 * size + 0x40,
                              GetClut(0x20, palette), 1, alpha, 0);
            }
            D_800F3368.extent_x = D_800F3368.parameter00;
            D_800F3368.extent_y = D_800F3368.parameter00;
            {
                RoomM404DropMatrixSlot *slot = &D_800BCFA4;
                gte_ldrotmatrix(slot->value);
                gte_ldtransmatrix(slot->value);
            }
            if (func_80071A54() & 3) return 0;
            func_800D004C(drop, 0x140, 0x140, 6, &rotation, 0x1000, 0x1000, &color, 0,
                          alpha / 2, 1);
            break;
        }
        break;
    }
    return 0;
}
