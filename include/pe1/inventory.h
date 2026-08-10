#ifndef PE1_INVENTORY_H
#define PE1_INVENTORY_H

#include "common.h"

/* Common 0x20-byte item-data record returned by Inv_LookupActiveListData.
 * Equipment, consumables and key items reuse the tail differently, so keep
 * the unproven portion generic instead of forcing a single variant's names.
 */
typedef struct ItemDataRecord {
/* 0x00 */ u8  pad_00[4];
/* 0x04 */ u8  itemId;
/* 0x05 */ u8  flags;
/* 0x06 */ u8  kind;
/* 0x07 */ u8  baseStats[3];
/* 0x0A */ u16 ammo;
/* 0x0C */ u16 reserveAmmo;
/* 0x0E */ s16 bonusStats[3];
/* 0x14 */ u8  tailCount;    /* use count or modifier-entry count by item kind */
/* 0x15 */ u8  tailData[0x0B];
} ItemDataRecord;

/* Historical name retained while older consumers are migrated. */
typedef ItemDataRecord InvItemSlot;

extern ItemDataRecord g_InvCompareSlotLeft;
extern ItemDataRecord g_InvCompareSlotRight;
extern ItemDataRecord D_800A1A00;
extern ItemDataRecord g_InvItemSlotArray[128];

ItemDataRecord *Inv_LookupActiveListData(int index);

PE1_STATIC_ASSERT(sizeof(ItemDataRecord) == 0x20, item_data_record_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(ItemDataRecord, itemId) == 0x04,
                  item_data_record_id_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(ItemDataRecord, flags) == 0x05,
                  item_data_record_flags_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(ItemDataRecord, kind) == 0x06,
                  item_data_record_kind_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(ItemDataRecord, ammo) == 0x0A,
                  item_data_record_ammo_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(ItemDataRecord, bonusStats) == 0x0E,
                  item_data_record_bonus_stats_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(ItemDataRecord, tailCount) == 0x14,
                  item_data_record_tail_offset);

#endif /* PE1_INVENTORY_H */
