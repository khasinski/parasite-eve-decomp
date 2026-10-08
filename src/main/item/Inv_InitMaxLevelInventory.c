#include "common.h"
#include "pe1/aya.h"
#include "pe1/inventory_slots.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

void Battle_SyncEquipSlots(void);
void *Aya_GetLevelExpTable(void);
extern u16 g_AyaHpMax[];
#define g_AyaHpMax (g_AyaHpMax[0])
extern u16 g_AyaHpCurrent[];
#define g_AyaHpCurrent (g_AyaHpCurrent[0])
extern s8 g_AyaInventorySlotCount[];
#define g_AyaInventorySlotCount (g_AyaInventorySlotCount[0])
extern s32 g_AyaParasiteSpellFlags[];
#define g_AyaParasiteSpellFlags (g_AyaParasiteSpellFlags[0])
extern u8 g_AyaSaveLevel[];
extern u8 g_AyaSaveLevel_read[] asm("g_AyaSaveLevel");
#define g_AyaSaveLevel (g_AyaSaveLevel[0])
extern s32 g_AyaSaveTotalExp[];
#define g_AyaSaveTotalExp (g_AyaSaveTotalExp[0])

void Inv_InitMaxLevelInventory(s32 arg0) {
    u16 *nextStat;
    u16 *stat;
    register s16 allocation asm("$2");
    s32 category;
    u16 maxHp;
    s32 mode;
    s16 *items;
    AyaSaveState *save;
    s32 unlockedSpells;
    AyaLevelStats *levelStats;
    s32 *expTable;

    mode = arg0;
    items = g_AyaInventoryItems;
    g_AyaInventorySlotCount = 0x32;
    if (g_InvItemPtr == items) {
        g_InvSlotLimit = Inv_GetAyaSlotLimit();
    }
    g_AyaSaveLevel = 0x62;
    expTable = (s32 *)Aya_GetLevelExpTable();
    g_AyaSaveTotalExp = expTable[g_AyaSaveLevel_read[0]];
    levelStats = Aya_LookupLevelStats(0x62);
    unlockedSpells = 0xFFFFF;
    save = (AyaSaveState *)((u8 *)items - PE1_OFFSETOF(AyaSaveState, inventory_items));
    nextStat = save->stat_allocations;
    maxHp = levelStats->hp;
    category = 0;
    g_AyaParasiteSpellFlags = unlockedSpells;
    g_AyaHpCurrent = maxHp;
    g_AyaHpMax = maxHp;
    __asm__ volatile("" ::: "memory");
    stat = nextStat;
    do {
        nextStat = stat + 1;
        if (mode == 0) {
            allocation = 0x3E8;
        } else {
            register s32 combatCategory asm("$2");
            combatCategory = (u32)(category - 1) < 2;
            if (combatCategory != 0) {
                allocation = 0;
            } else {
                allocation = 0x3E8;
            }
        }
        *stat = allocation;
        category += 1;
        stat = nextStat;
    } while (category < 7);
    Battle_SyncEquipSlots();
}
