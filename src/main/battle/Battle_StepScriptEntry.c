/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

typedef signed char s8;
typedef unsigned char u8;
typedef unsigned int u32;

extern struct { char _[16]; } D_8009D254_o __asm__("D_8009D254");
extern struct { char _[16]; } D_8009D278_o __asm__("D_8009D278");
extern struct { char _[16]; } D_8009D2E8_o __asm__("D_8009D2E8");
extern struct { char _[16]; } D_8009D28C_o __asm__("D_8009D28C");
extern struct { char _[16]; } D_800BE830_o __asm__("D_800BE830");

#define D_8009D254 (*(u8 **)&D_8009D254_o)
#define D_8009D278 (*(u8 **)&D_8009D278_o)
#define D_8009D2E8 (*(u32 *)&D_8009D2E8_o)
#define D_8009D28C (*(u32 *)&D_8009D28C_o)
#define BATTLE_QUEUE_ENTRY(index) ((u8 *)&D_800BE830_o + ((u8)(index) << 3))

extern u8 D_8009D1D4;
extern u8 D_8009CE3C;

void Battle_UpdatePlayerTurn(void);
void Battle_ApplyPlayerHit(void);
void Battle_AdvancePhase(void);
void Entity_SetActionMode(u8 *entity, int mode);
int Battle_RollEscapeChance(void);

void Battle_StepScriptEntry(void) {
    u8 index = D_8009D1D4;
    u8 *entity;
    int action;

    if (index >= D_8009CE3C) {
        D_8009D1D4 = 0;
        D_8009CE3C = 0;
        return;
    }

    entity = D_8009D254;
    *(u32 *)(entity + 0x68) = 0;
    *(u32 *)(entity + 0x6C) = 0;
    *(u32 *)(entity + 0x70) = 0;

    if (entity[0x0E] < 4) {
        return;
    }

    action = *(short *)(BATTLE_QUEUE_ENTRY(index) + 4);
    if (action < 3) {
        Battle_UpdatePlayerTurn();
        return;
    }

    if (action < 0x183) {
        Battle_ApplyPlayerHit();
        return;
    }

    if (action < 0x197) {
        Battle_AdvancePhase();
        return;
    }

    if (action < 0x199) {
        Entity_SetActionMode(entity, 0xD);
        D_8009D2E8 |= 1;
        D_8009D1D4++;
        *(u32 *)(D_8009D254 + 0x98) |= 0x100;
        return;
    }

    if ((s8)Battle_RollEscapeChance() == 1) {
        Entity_SetActionMode(D_8009D254, D_8009D278[0x12]);
        D_8009D28C = 4;
    }

    D_8009D1D4++;
}
