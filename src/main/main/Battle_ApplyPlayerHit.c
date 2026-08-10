#include "common.h"
#include "pe1/battle.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

extern struct { char _[16]; } D_8009D254_a __asm__("D_8009D254");
extern struct { char _[16]; } D_8009D254_b __asm__("D_8009D254");
extern struct { char _[16]; } D_8009D254_c __asm__("D_8009D254");
extern struct { char _[16]; } D_8009D254_d __asm__("D_8009D254");
extern struct { char _[16]; } D_8009D254_e __asm__("D_8009D254");
extern struct { char _[16]; } D_8009D278_a __asm__("D_8009D278");
extern struct { char _[16]; } D_8009D278_b __asm__("D_8009D278");
extern struct { char _[16]; } D_8009D278_c __asm__("D_8009D278");
extern struct { char _[16]; } D_8009D278_d __asm__("D_8009D278");
extern struct { char _[16]; } D_8009D1A0_r0 __asm__("D_8009D1A0");
extern struct { char _[16]; } D_8009D1A0_w0 __asm__("D_8009D1A0");
extern struct { char _[16]; } D_8009D1A0_r1 __asm__("D_8009D1A0");
extern struct { char _[16]; } D_8009D1A0_w1 __asm__("D_8009D1A0");
extern u8 D_8009D1D4;
extern u16 D_800BE834[];

#define D254_A (*(BattleEntity **)&D_8009D254_a)
#define D254_B (*(BattleEntity **)&D_8009D254_b)
#define D254_C (*(BattleEntity **)&D_8009D254_c)
#define D254_D (*(BattleEntity **)&D_8009D254_d)
#define D254_E (*(BattleEntity **)&D_8009D254_e)
#define D278_A (*(Combatant **)&D_8009D278_a)
#define D278_B (*(Combatant **)&D_8009D278_b)
#define D278_C (*(Combatant **)&D_8009D278_c)
#define D278_D (*(Combatant **)&D_8009D278_d)
#define D1A0_R0 (*(int *)&D_8009D1A0_r0)
#define D1A0_W0 (*(int *)&D_8009D1A0_w0)
#define D1A0_R1 (*(int *)&D_8009D1A0_r1)
#define D1A0_W1 (*(int *)&D_8009D1A0_w1)

#define U8_AT(ptr, offset) (*(u8 *)((u8 *)(ptr) + (offset)))
#define S16_AT(ptr, offset) (*(s16 *)((u8 *)(ptr) + (offset)))
#define U16_AT(ptr, offset) (*(u16 *)((u8 *)(ptr) + (offset)))
#define U32_AT(ptr, offset) (*(u32 *)((u8 *)(ptr) + (offset)))
#define ACTION_AT(index) (*(u16 *)((u8 *)D_800BE834 + ((u8)(index) << 3)))

void Entity_SetActionMode(void *entity, int mode);
void Asset_Find08Alt(int id, int arg1, int x, int y, int z);
void Battle_ApplyDamage(int action);
int Scene_LoadRoomAssets(int room, void *entity);

void Battle_ApplyPlayerHit(void) {
    u32 committed;
    register int mask asm("$5");
    register unsigned int index asm("$3");
    Combatant *actor;
    int flags;
    u16 action;

    committed = 0x200000;
    if (D278_A->stateFlags & committed) {
        Entity_SetActionMode(D254_A, 0xE);
        Asset_Find08Alt(0x4B3, 0, D254_B->posX.parts.integer,
                        D254_B->posY.parts.integer, D254_B->posZ.parts.integer);

        mask = ~0x200000;
        actor = D278_B;
        index = actor->stateFlags;
        flags = D1A0_R0;
        D1A0_W0 = flags | 0x100;
        actor->stateFlags = index & mask;
    }

    if (D254_C->animLastFrame == D254_C->animPrev.parts.integer) {
        Entity_SetActionMode(D254_C, D278_C->actionMode12);
        actor = D278_D;
        index = D_8009D1D4;
        actor->stateFlags |= committed;

        Battle_ApplyDamage((s16)ACTION_AT(index) - 3);
        Scene_LoadRoomAssets(0x55, D254_D);

        index = D_8009D1D4;
        index++;
        action = ACTION_AT(index);

        D_8009D1D4 = index;

        if ((unsigned int)(action - 3) >= 0x194 || D254_E->actionMode < 4) {
            D1A0_W1 = D1A0_R1 & ~0x100;
        }
    }
}
