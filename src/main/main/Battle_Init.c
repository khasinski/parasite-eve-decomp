/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

typedef unsigned char u8;
typedef unsigned int u32;

extern struct { char _[16]; } D_8009D278_o __asm__("D_8009D278");
extern struct { char _[16]; } D_8009D2FC_o __asm__("D_8009D2FC");
extern struct { char _[16]; } D_8009D1AC_o __asm__("D_8009D1AC");
extern struct { char _[16]; } D_8009D1CE_o __asm__("D_8009D1CE");
extern struct { char _[16]; } D_800BE830_o __asm__("D_800BE830");

#define D_8009D278 (*(u8 **)&D_8009D278_o)
#define D_8009D2FC (*(int *)&D_8009D2FC_o)
#define D_8009D1AC (*(u32 *)&D_8009D1AC_o)
#define D_8009D1CE (*(u8 *)&D_8009D1CE_o)
#define BATTLE_QUEUE ((u8 *)&D_800BE830_o)

extern int D_8009D200;
extern int D_8009D258;
extern int D_8009D208;
extern u8 D_8009CE44;
extern u8 D_8009CE40;
extern u8 D_8009D294;
extern u8 D_8009D2D8;
extern u8 D_8009D1DC;
extern u8 D_8009CE38;
extern u8 D_8009CE39;
extern u8 D_8009CE3A;
extern u8 D_8009CE3B;
extern u8 D_8009CE60;

void Pm_StopAll(void);
void Tbl_ResetAll(void);
void Battle_InitFadeVars(void);

void Battle_Init(void) {
    int i;
    u8 *entry;
    u8 *actor;
    u8 *action;
    u32 turn_word;

    Pm_StopAll();

    D_8009D200 = -1;
    D_8009D2FC = -1;
    D_8009D258 = -1;
    D_8009D208 = -1;

    for (i = 0; (u8)i < 0x2D; i++) {
        entry = BATTLE_QUEUE + (((u8)i) << 3);
        *(u32 *)entry = 0;
        *(short *)(entry + 6) = 0;
        *(short *)(entry + 4) = 0;
    }

    actor = D_8009D278;
    action = *(u8 **)(actor + 0x68);

    D_8009CE44 = 0;
    D_8009CE40 = 0;
    D_8009D294 = 0;

    turn_word = *(u32 *)(action + 0x10);
    D_8009D2D8 = (turn_word >> 4) & 3;

    action = *(u8 **)(actor + 0x68);
    turn_word = *(u32 *)(action + 0x10);
    D_8009D1DC = turn_word & 0xF;

    action = *(u8 **)(actor + 0x68);
    D_8009CE38 = *(u8 *)(action + 0x14);
    D_8009CE39 = *(u8 *)(*(u8 **)(actor + 0x68) + 0x15);
    D_8009CE3A = *(u8 *)(*(u8 **)(actor + 0x68) + 0x16);
    D_8009CE3B = *(u8 *)(*(u8 **)(actor + 0x68) + 0x17);

    Tbl_ResetAll();

    D_8009D1CE = 0;
    D_8009D1AC &= ~0x300U;

    Battle_InitFadeVars();

    D_8009CE60 = 0;
}
