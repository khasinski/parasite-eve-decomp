#include "common.h"

/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

typedef struct BattleInitRecord {
    u32 value;
    u16 state;
    u16 timer;
} BattleInitRecord;

typedef struct GlobalWordObject {
    s32 value;
    u8 pad[12];
} GlobalWordObject;

typedef struct GlobalByteObject {
    u8 value;
    u8 pad[15];
} GlobalByteObject;

typedef struct BattleRootObject {
    u8 *root;
    u8 pad[12];
} BattleRootObject;

extern BattleInitRecord D_800BE830[45];
extern s32 D_8009D200;
extern GlobalWordObject D_8009D2FC_o __asm__("D_8009D2FC");
extern s32 D_8009D258;
extern s32 D_8009D208;
extern u8 D_8009CE44;
extern u8 D_8009CE40;
extern u8 D_8009D294;
extern u8 D_8009D2D8;
extern u8 D_8009D1DC;
u8 D_8009CE38;
u8 D_8009CE39;
u8 D_8009CE3A;
u8 D_8009CE3B;
extern u8 D_8009CE60;
extern BattleRootObject D_8009D278_o __asm__("D_8009D278");
extern GlobalWordObject D_8009D1AC_read __asm__("D_8009D1AC");
extern GlobalWordObject D_8009D1AC_write __asm__("D_8009D1AC");
extern GlobalByteObject D_8009D1CE_o __asm__("D_8009D1CE");

void Pm_StopAll(void);
void Tbl_ResetAll(void);
void Battle_InitFadeVars(void);

void Battle_Init(void)
{
    u8 index;
    BattleInitRecord *records;
    u8 *root;
    u8 *config0;
    u8 *config1;
    volatile u8 *config2;
    volatile u8 *config3;
    volatile u8 *config4;
    volatile u8 *config5;

    Pm_StopAll();
    index = 0;
    records = D_800BE830;
    D_8009D200 = -1;
    D_8009D2FC_o.value = -1;
    D_8009D258 = -1;
    D_8009D208 = -1;

    do {
        D_800BE830[index].value = 0;
        records[index].timer = 0;
        D_800BE830[index].state = 0;
        index++;
    } while (index < 45);

    root = D_8009D278_o.root;
    config0 = *(u8 **)(root + 0x68);
    D_8009CE44 = 0;
    D_8009CE40 = 0;
    D_8009D294 = 0;
    config1 = *(u8 **)(root + 0x68);
    D_8009D2D8 = (*(u32 *)(config0 + 0x10) >> 4) & 3;
    config2 = *(u8 **)(root + 0x68);
    D_8009D1DC = *(u32 *)(config1 + 0x10) & 0xF;
    D_8009CE38 = config2[0x14];
    config3 = *(u8 **)(root + 0x68);
    D_8009CE39 = config3[0x15];
    config4 = *(u8 **)(root + 0x68);
    D_8009CE3A = config4[0x16];
    config5 = *(u8 **)(root + 0x68);
    D_8009CE3B = config5[0x17];

    Tbl_ResetAll();
    D_8009D1CE_o.value = 0;
    D_8009D1AC_write.value = D_8009D1AC_read.value & ~0x300;
    Battle_InitFadeVars();
    D_8009CE60 = 0;
}
