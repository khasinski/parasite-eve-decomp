#include "common.h"

#define U8_AT(ptr, off) (*(u8 *)((u8 *)(ptr) + (off)))
#define S16_AT(ptr, off) (*(s16 *)((u8 *)(ptr) + (off)))
#define U32_AT(ptr, off) (*(u32 *)((u8 *)(ptr) + (off)))
#define S32_AT(ptr, off) (*(s32 *)((u8 *)(ptr) + (off)))
#define PTR_AT(ptr, off) (*(u8 **)((u8 *)(ptr) + (off)))

typedef struct GlobalPointerSlot {
    u8 *value;
    u8 pad[8];
} GlobalPointerSlot;

extern u8 *D_8009D20C[];
extern GlobalPointerSlot D_8009D254;

int Entity_GetPositionByType(int **args) {
    u8 *entity;
    u8 *player;
    int selector;
    int entity_id;

    selector = *args[1];
    if (selector == 0) {
        player = D_8009D254.value;
        if (player == 0) {
            goto not_found;
        }
        entity = player;
        goto found;
    } else {
        entity = D_8009D20C[0];
        entity_id = selector;
        while (entity != 0) {
            if (U8_AT(entity, 0xC) == entity_id &&
                U8_AT(entity, 0xD) == *args[2] &&
                (U32_AT(entity, 0x98) & 0x10) == 0) {
                break;
            }
            entity = PTR_AT(entity, 4);
        }
        if (entity == 0) {
            goto not_found;
        }
    }

    goto found;

not_found:
    *args[6] = -1;
    return 1;

found:
    *args[6] = 1;
    switch (*args[0]) {
    case 0:
        *args[3] = S32_AT(entity, 0x28);
        *args[4] = S32_AT(entity, 0x2C);
        *args[5] = S32_AT(entity, 0x30);
        break;
    case 1:
        *args[3] = S32_AT(entity, 0x40);
        *args[4] = S32_AT(entity, 0x44);
        *args[5] = S32_AT(entity, 0x48);
        break;
    case 2:
        *args[3] = S32_AT(entity, 0x68);
        *args[4] = S32_AT(entity, 0x6C);
        *args[5] = S32_AT(entity, 0x70);
        break;
    case 3:
        *args[3] = S32_AT(entity, 0x78);
        *args[4] = S32_AT(entity, 0x7C);
        *args[5] = S32_AT(entity, 0x80);
        break;
    case 4:
        *args[3] = S32_AT(entity, 0x88);
        *args[4] = S32_AT(entity, 0x8C);
        *args[5] = S32_AT(entity, 0x90);
        break;
    case 5:
        *args[3] = S16_AT(entity, 0x38);
        *args[4] = S16_AT(entity, 0x3A);
        *args[5] = S16_AT(entity, 0x3C);
        break;
    case 6:
        *args[3] = S32_AT(entity, 0x58);
        *args[4] = S32_AT(entity, 0x5C);
        *args[5] = S32_AT(entity, 0x60);
        break;
    }
    return 1;
}
