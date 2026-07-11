/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
typedef signed short s16;
typedef unsigned char u8;
typedef unsigned int u32;

#define S16_AT(ptr, offset) (*(s16 *)((u8 *)(ptr) + (offset)))
#define U32_AT(ptr, offset) (*(u32 *)((u8 *)(ptr) + (offset)))

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

#define D_8009D254_A (*(u8 **)&D_8009D254_a)
#define D_8009D254_B (*(u8 **)&D_8009D254_b)
#define D_8009D254_C (*(u8 **)&D_8009D254_c)
#define D_8009D254_D (*(u8 **)&D_8009D254_d)
#define D_8009D254_E (*(u8 **)&D_8009D254_e)
#define D_8009D254_F (*(u8 **)&D_8009D254_f)
#define D_8009D254_G (*(u8 **)&D_8009D254_g)
#define D_8009D278_A (*(u8 **)&D_8009D278_a)
#define D_8009D278_B (*(u8 **)&D_8009D278_b)
#define D_8009D2FC (*(int *)&D_8009D2FC_o)

extern int D_8009D200;

int BattleCmd_CommitAmmoAndUpdate(void);
int Scene_LoadRoomAssets(int id, void *entity);

void Battle_DispatchEntityEffect(void) {
    u8 *actor;
    u8 *action;
    u8 *next_entity;
    int action_id;
    int next_id;
    int result;
    u32 word10;

    actor = D_8009D278_A;
    action = *(u8 **)(actor + 0x68);
    action_id = S16_AT(action, 0x6);

    if (action_id == 6) {
        if ((U32_AT(action, 0xC) & 0x3FF) == 0) {
            BattleCmd_CommitAmmoAndUpdate();
        }
        D_8009D200 = Scene_LoadRoomAssets(3, D_8009D254_A);
        return;
    }

    if (action_id == 8) {
        D_8009D2FC = Scene_LoadRoomAssets(6, D_8009D254_B);
        return;
    }

    if ((U32_AT(action, 0xC) & 0x3FF) == 0) {
        BattleCmd_CommitAmmoAndUpdate();
    }

    actor = D_8009D278_B;
    action = *(u8 **)(actor + 0x68);
    action_id = S16_AT(action, 0x6);

    if (action_id == 5) {
        goto load_pair5;
    }

    word10 = U32_AT(action, 0x10);
    if ((word10 & 0x1F00) == 0) {
        goto choose_first;
    }

load_pair5:
    result = Scene_LoadRoomAssets(0, D_8009D254_E);
    next_entity = D_8009D254_F;
    next_id = 5;
    goto load_next;

choose_first:
    if ((action_id == 2) || ((word10 & 0xC0) == 0x80)) {
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
