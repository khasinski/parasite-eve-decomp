#include "common.h"
#include "pe1/battle.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#define NULL ((void *)0)
s32 Battle_CalcDistToPlayer();
s32 ratan2();
extern s8 g_BattleTargetIndex;
extern struct { char _[16]; } D_8009D20C_o __asm__("g_FieldActorListHead");
#define g_FieldActorListHead (*(void **)&D_8009D20C_o)
extern struct { char _[16]; } g_PlayerEntity_o __asm__("g_PlayerEntity");
extern struct { char _[16]; } g_PlayerEntity_o2 __asm__("g_PlayerEntity");
extern struct { char _[16]; } g_PlayerEntity_o3 __asm__("g_PlayerEntity");
#define g_PlayerEntity (*(void **)&g_PlayerEntity_o)
#define g_PlayerEntity_2 (*(void **)&g_PlayerEntity_o2)
#define g_PlayerEntity_3 (*(void **)&g_PlayerEntity_o3)
extern struct { char _[16]; } D_8009E000_o __asm__("g_BattleTargetList");
extern struct { char _[16]; } D_8009E004_o __asm__("g_BattleTargetDistance");
extern struct { char _[16]; } D_8009E008_o __asm__("D_8009E008");
#define E000_AT(off) (*(void **)((u8 *)&D_8009E000_o + (off)))
#define E004_AT(off) (*(s32 *)((u8 *)&D_8009E004_o + (off)))
#define E008_AT(off) (*(s16 *)((u8 *)&D_8009E008_o + (off)))

void Battle_BuildTargetList(void) {
    s32 entityFlags;
    s32 off;
    s32 off1;
    register s32 nextIndex asm("$5");
    register s32 finalCount asm("$2");
    s32 pad_[2];
    s32 distance;
    s16 angle;
    register EnemyCombatant *candidateCore asm("$6");
    register BattleEntity *player asm("$5");
    BattleEntity *candidate;

    candidate = g_FieldActorListHead;
    g_BattleTargetIndex = 0;
    if (candidate != NULL) {
        do {
            player = g_PlayerEntity;
            if (candidate != player) {
                candidateCore = candidate->core;
                if (candidateCore != NULL) {
                    entityFlags = candidate->entityFlags;
                    if (((entityFlags & 0x2040) != 0x40) && !(entityFlags & 0x4000) &&
                        (candidateCore->hpAlive > 0)) {
                        off1 = g_BattleTargetIndex * 0xC;
                        E000_AT(off1) = candidate;
                        distance = Battle_CalcDistToPlayer(candidate);
                        off = g_BattleTargetIndex * 0xC;
                        E004_AT(off) = distance;
                        angle = ratan2(
                            candidate->renderObject.target_x -
                                ((BattleEntity *)g_PlayerEntity_2)->posX.parts.integer,
                            candidate->renderObject.target_z -
                                ((BattleEntity *)g_PlayerEntity_2)->posZ.parts.integer);
                        nextIndex = g_BattleTargetIndex + 1;
                        off = g_BattleTargetIndex * 0xC;
                        E008_AT(off) = angle;
                        g_BattleTargetIndex = nextIndex;
                    }
                }
            }
            candidate = candidate->next;
        } while (candidate != NULL);
    }
    if (g_BattleTargetIndex >= 2) {
        Battle_SortTargets((BattleTargetWords *)&D_8009E000_o, 0, (s8) (g_BattleTargetIndex - 1));
    }
    finalCount = g_BattleTargetIndex;
    __asm__("" : "=r"(finalCount) : "0"(finalCount));
    off = finalCount * 0xC;
    E000_AT(off) = NULL;
}
