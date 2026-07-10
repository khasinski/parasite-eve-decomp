#include "include_asm.h"

typedef signed char s8;
typedef signed short s16;
typedef unsigned char u8;
typedef unsigned int u32;

extern void *D_8009D20C;
extern void *D_8009D254;
extern void *D_8009D278;
extern char D_8009D1CE;
extern void *D_8009D1F8;
extern char D_800915C0[];
extern char D_8009159C[];

int rand(void);
int Save_GetMetadataWindowIndex(void);

int Battle_RollEscapeChance(void) {
    void *node;
    void *actor;
    void *active_node;
    void *active_actor;
    s8 chance;
    s16 max_rank;
    s8 rank_diff;
    int status;
    int index;
    int roll;
    u32 flags;

    node = D_8009D20C;
    chance = 0;
    max_rank = 0;
    if (node != 0) {
        active_node = D_8009D254;
        do {
            if (node != active_node) {
                actor = *(void **)node;
                if (actor != 0 && *(int *)((char *)actor + 0x10) > 0) {
                    if (max_rank < (s8)*(u8 *)((char *)actor + 4)) {
                        max_rank = (s8)*(u8 *)((char *)actor + 4);
                    }

                    if (*(u32 *)((char *)actor + 0xCC) & 0x40000) {
                        chance = -1;
                        break;
                    }
                }
            }
            node = *(void **)((char *)node + 4);
        } while (node != 0);
    }

    if (chance == 0) {
        active_actor = D_8009D278;
        rank_diff = (s8)(*(u8 *)((char *)active_actor + 4) - max_rank);
        if (rank_diff >= 2) {
            chance = 0x50;
        } else if (rank_diff >= 0) {
            chance = 0x28;
        } else if (rank_diff == -1) {
            chance = 0x19;
        } else {
            chance = 0x0F;
        }

        if ((*(s16 *)((char *)active_actor + 0x0C) * 10) < *(s16 *)((char *)active_actor + 0x1C)) {
            chance = (chance * 3) / 2;
        }

        status = (*(u32 *)((char *)active_actor + 0x4C) >> 25) & 7;
        switch (status) {
        case 1:
            chance = (chance * 3) / 2;
            break;
        case 2:
            chance *= 2;
            break;
        case 3:
            chance *= 3;
            break;
        case 4:
            chance *= 4;
            break;
        case 5:
            chance = 100;
            break;
        }

        roll = rand() % 100;
        chance = roll < chance;
    }

    if (chance <= 0) {
        D_8009D1CE = 1;
        if (chance == -1) {
            index = Save_GetMetadataWindowIndex();
            D_8009D1F8 = D_800915C0 + (index * 14);
        } else {
            index = Save_GetMetadataWindowIndex();
            D_8009D1F8 = D_8009159C + (index * 17);
        }

        active_actor = D_8009D278;
        flags = *(u32 *)((char *)active_actor + 0x4C);
        status = (flags >> 25) & 7;
        if (status < 5) {
            flags &= 0xF1FFFFFF;
            flags |= ((status + 1) & 7) << 25;
            *(u32 *)((char *)active_actor + 0x4C) = flags;
        }
    }

    return (s8)chance;
}
