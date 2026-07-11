/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
typedef signed short s16;
typedef unsigned char u8;
typedef unsigned int u32;

#define S16_AT(ptr, offset) (*(s16 *)((u8 *)(ptr) + (offset)))
#define U32_AT(ptr, offset) (*(u32 *)((u8 *)(ptr) + (offset)))

extern struct { char _[16]; } D_8009D254_o __asm__("D_8009D254");
extern struct { char _[16]; } D_8009D278_o __asm__("D_8009D278");
extern int D_8009D200;
extern u8 D_8009D294;

#define D_8009D254 (*(u8 **)&D_8009D254_o)
#define D_8009D278 (*(u8 **)&D_8009D278_o)

void Asset_Find08Alt(int id, int arg1, int x, int y, int z);
int Akao_SetPos3D(int arg0, int arg1, int x, int y, int z);
int Pm_SendCmd(int arg0, int arg1, int arg2, int arg3, int arg4, int arg5);

void Battle_FinalizeAttackResult(void) {
    u8 *actor;
    u8 *action;
    u8 *player;
    u32 attack_word;

    actor = D_8009D278;
    action = *(u8 **)(actor + 0x68);

    if (S16_AT(action, 0x6) == 8) {
        player = D_8009D254;
        D_8009D294 = 1;
        Asset_Find08Alt(0x46C, 0, S16_AT(player, 0x2A), S16_AT(player, 0x2E), S16_AT(player, 0x32));
        return;
    }

    attack_word = U32_AT(action, 0xC);
    if ((attack_word & 0x3FF) != 0) {
        D_8009D294 = 1;

        if ((U32_AT(actor, 0x4C) & 0x100000) == 0) {
            Pm_SendCmd(D_8009D200, 0, 0, 1, 0, 0);
            player = D_8009D254;
            Akao_SetPos3D(0, 0, S16_AT(player, 0x2A), S16_AT(player, 0x2E), S16_AT(player, 0x32));
        }

        action = *(u8 **)(D_8009D278 + 0x68);
        attack_word = U32_AT(action, 0xC);
        U32_AT(action, 0xC) = (attack_word & ~0x3FF) | (((attack_word & 0x3FF) - 1) & 0x3FF);
        return;
    }

    player = D_8009D254;
    Asset_Find08Alt(0x46E, 0, S16_AT(player, 0x2A), S16_AT(player, 0x2E), S16_AT(player, 0x32));
    D_8009D294 = 0;
}
