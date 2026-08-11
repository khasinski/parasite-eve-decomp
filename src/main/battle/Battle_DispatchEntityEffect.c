#include "common.h"
#include "pe1/battle.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

extern struct { char _[16]; } D_8009D254_a __asm__("D_8009D254");
extern struct { char _[16]; } D_8009D254_b __asm__("D_8009D254");
extern struct { char _[16]; } D_8009D254_c __asm__("D_8009D254");
extern struct { char _[16]; } D_8009D254_d __asm__("D_8009D254");
extern struct { char _[16]; } D_8009D254_e __asm__("D_8009D254");
extern struct { char _[16]; } D_8009D254_f __asm__("D_8009D254");
extern struct { char _[16]; } D_8009D254_g __asm__("D_8009D254");
extern struct { char _[16]; } D_8009D278_a __asm__("D_8009D278");
extern struct { char _[16]; } D_8009D278_b __asm__("D_8009D278");
extern struct { char _[16]; } D_8009D2FC_o __asm__("D_8009D2FC");

#define D_8009D254_A (*(BattleEntity **)&D_8009D254_a)
#define D_8009D254_B (*(BattleEntity **)&D_8009D254_b)
#define D_8009D254_C (*(BattleEntity **)&D_8009D254_c)
#define D_8009D254_D (*(BattleEntity **)&D_8009D254_d)
#define D_8009D254_E (*(BattleEntity **)&D_8009D254_e)
#define D_8009D254_F (*(BattleEntity **)&D_8009D254_f)
#define D_8009D254_G (*(BattleEntity **)&D_8009D254_g)
#define D_8009D278_A (*(Combatant **)&D_8009D278_a)
#define D_8009D278_B (*(Combatant **)&D_8009D278_b)
#define D_8009D2FC (*(int *)&D_8009D2FC_o)

extern int D_8009D200;

int BattleCmd_CommitAmmoAndUpdate(void);
int Scene_LoadRoomAssets(int id, void *entity);

void Battle_DispatchEntityEffect(void) {
    Combatant *actor;
    BattleAction *action;
    BattleAction *action2;
    BattleEntity *next_entity;
    register int action_id asm("$3");
    int action_id2;
    int next_id;
    int result;
    u32 word10;

    actor = D_8009D278_A;
    action = actor->action;
    action_id = action->actionCode.actionId;

    if (action_id == 6) {
        if ((action->attackWord & 0x3FF) == 0) {
            BattleCmd_CommitAmmoAndUpdate();
        }
        D_8009D200 = Scene_LoadRoomAssets(3, D_8009D254_A);
        return;
    }

    if (action_id == 8) {
        D_8009D2FC = Scene_LoadRoomAssets(6, D_8009D254_B);
        return;
    }

    if ((action->attackWord & 0x3FF) == 0) {
        BattleCmd_CommitAmmoAndUpdate();
    }

    actor = D_8009D278_B;
    action2 = actor->action;
    action_id2 = action2->actionCode.actionId;

    if (action_id2 == 5) {
        goto load_pair5;
    }

    word10 = action2->turnWord;
    if ((word10 & 0x1F00) == 0) {
        goto choose_first;
    }

load_pair5:
    result = Scene_LoadRoomAssets(0, D_8009D254_E);
    next_entity = D_8009D254_F;
    next_id = 5;
    goto load_next;

choose_first:
    if ((action_id2 == 2) || ((word10 & 0xC0) == 0x80)) {
        result = Scene_LoadRoomAssets(1, D_8009D254_C);
    } else {
        result = Scene_LoadRoomAssets(2, D_8009D254_G);
    }
    next_entity = D_8009D254_D;
    next_id = 4;

load_next:
    D_8009D200 = result;
    D_8009D2FC = Scene_LoadRoomAssets(next_id, next_entity);
}
