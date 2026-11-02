/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
/* MASPSX_FORCE_G0: 1 */

#include "common.h"
#include "pe1/akao.h"
#include "pe1/akao/commands.h"
#include "pe1/akao_counters.h"
#include "pe1/akao_send.h"
#include "pe1/aya.h"
#include "pe1/battle_modifiers.h"
#include "pe1/draw_state.h"
#include "pe1/game_state.h"
#include "pe1/game_timers.h"
#include "pe1/inventory.h"
#include "pe1/inventory_category.h"
#include "pe1/inventory_queries.h"
#include "pe1/inventory_slots.h"
#include "pe1/inventory_sort.h"
#include "pe1/inventory_state.h"
#include "pe1/menu_context_help.h"
#include "pe1/menu_inventory.h"
#include "pe1/menu_queue.h"
#include "pe1/menu_state.h"
#include "pe1/random.h"
#include "pe1/save.h"
#include "pe1/script_queue.h"
#include "pe1/text.h"

/* COMMON supplies small-data metadata to MASPSX: these words are reached
 * through $gp, every other extern stays absolute. */
s32 g_ItemDropRngIndex;
s32 g_InvPackedListCount;
s16 *g_InvItemPtr;
s32 g_InvSlotLimit;
u32 *g_InvSelectionBits;
s32 g_InvSelectionBitWords;
s32 g_InvPackedListCursor;
int g_SaveMetadataWindowIndex;
s16 *g_InvActiveListOverride;
s32 g_InvOverrideSlotLimit;
int g_AkaoPendingBgmVolumeFade;
int g_MemCardDialogState;
unsigned int g_MenuActionSubmenuSel;
unsigned int g_MenuErrorSoundPending;
SavedMenuMode g_SavedMenuMode;
int g_InvCategoryBaseItemId;

void Akao_SetBgmVolumeFade(void) {
    int value;

    if (g_AkaoBgmHandle[0] != 0) {
        value = Akao_SendTableCommand(g_AkaoBgmHandle[0], 0x450, 0x100, 0x80, 0x7F);
    } else {
        value = 0;
    }
    g_AkaoPendingBgmVolumeFade = value;
}

/* Retail passes the window index although the reset ignores it. */
void Save_ResetMetadataWindow(int);
void Menu_ResetStateFlags(void);
void Inv_SetDefaultItemId(void);
void MenuWidget_InitPoolUnk(void);
void Menu_InitStateBuffer(void);
void MemCard_ClearDelayedCallback(void);
void BattleCmd_InitTableCursor(void);
void Inv_BuildItemGridFromCategory(void);
void Menu_SetEquipSlotIndex(int index);

void Akao_FlushBgmVolumeFade(void) {
    if (g_AkaoPendingBgmVolumeFade != 0) {
        Akao_Cmd_21(g_AkaoPendingBgmVolumeFade, 0);
        g_AkaoPendingBgmVolumeFade = 0;
    }
}

void Menu_SetActionSubmenuSelection(int value) {
    g_MenuActionSubmenuSel = value;
    Akao_Cmd_80_81_82(value == 0);
}

unsigned int Menu_GetActionSubmenuSelection(void) {
    return g_MenuActionSubmenuSel;
}

void Inv_BoundsCheckAssertStub(int code)
{
}

void Menu_FullInit(void) {
    Menu_SetBattleEquipMode(0);
    Save_ResetMetadataWindow(0);
    g_SavedMenuMode.value = 0;
    g_MemCardDialogState = 0;
    g_MenuErrorSoundPending = 0;
    Menu_ResetStateFlags();
    Inv_SetDefaultItemId();
    Draw_InitBuffers();
    MenuWidget_InitPoolUnk();
    Menu_InitStateBuffer();
    MenuInput_InitQueue();
    MemCard_ClearDelayedCallback();
    BattleCmd_InitTableCursor();
    Inv_BuildItemGridFromCategory();
    Save_SelectMetadataWindow(0);
    Save_InitMetadataState();
    BattleCmd_LoadWeaponModifiers();
    Menu_SaveBgInitFade();
}

int GameTime_GetZero(void)
{
    return 0;
}

unsigned int GameTime_GetCounterSeconds(int timer) {
    return (unsigned int)g_GameTimers[timer].current / 60U;
}

void GameTime_SetCounterSeconds(int timer, int seconds) {
    g_GameTimers[timer].current = seconds * 60;
}

/* Item-drop random table: seeding from the play clock and the per-draw
 * lagged-xor refresh of the 0x209-byte table. */

#define SCRAMBLE_RANDOM_BYTES()                    \
    {                                           \
        int scramble_i_;                           \
        for (scramble_i_ = 0; scramble_i_ < 0x20; scramble_i_++) { \
            g_ItemDropRngTable[scramble_i_] ^= g_ItemDropRngXorKeyLow[scramble_i_]; \
        }                                          \
        for (scramble_i_ = 0x20; scramble_i_ < 0x209; scramble_i_++) { \
            g_ItemDropRngTable[scramble_i_] ^= g_ItemDropRngXorKeyHigh[scramble_i_]; \
        }                                          \
    }

void Inv_SetDefaultItemId(void) {
    u32 state[0x209];
    u32 seed;
    u32 bits;
    int bit;
    int index;

    seed = (unsigned int)g_GameTimers[0].current / 60;
    bits = 0;

    for (index = 0; index < 0x11; index++) {
        for (bit = 0x1F; bit >= 0; bit--) {
            seed = seed * 0x5D588B65 + 1;
            bits = (bits >> 1) | (seed & 0x80000000);
        }
        state[index] = bits;
    }

    state[0x10] = (state[0x10] << 23) ^ (state[0] >> 9) ^ state[0xF];

    for (index = 0x11; index < 0x209; index++) {
        state[index] = (state[index - 0x11] << 23) ^ (state[index - 0x10] >> 9) ^ state[index - 1];
    }

    for (index = 0; index < 0x209; index++) {
        g_ItemDropRngTable[index] = state[index];
    }

    SCRAMBLE_RANDOM_BYTES();
    SCRAMBLE_RANDOM_BYTES();
    SCRAMBLE_RANDOM_BYTES();

    g_ItemDropRngIndex = 0x208;
}

int Inv_ScrambleGrid(void)
{
    int i;

    g_ItemDropRngIndex++;
    if (g_ItemDropRngIndex >= 0x209) {
        for (i = 0; i < 0x20; i++) {
            g_ItemDropRngTable[i] = g_ItemDropRngTable[i] ^ g_ItemDropRngXorKeyLow[i];
        }
        for (i = 0x20; i < 0x209; i++) {
            g_ItemDropRngTable[i] = g_ItemDropRngTable[i] ^ g_ItemDropRngXorKeyHigh[i];
        }
        g_ItemDropRngIndex = 0;
    }

    return g_ItemDropRngTable[g_ItemDropRngIndex];
}

/* Text byte-string helpers, the item grid rebuild and the active inventory
 * list selection (Aya's list or a storage override) with its slot limit. */

void Text_CopyFFTerminatedBytes(u8 *dst, u8 *src)
{
    u8 first;

    first = *src++;
    *dst++ = first;
    if (first != 0xFF) {
        int term;
        int next;

        term = 0xFF;
        do {
            next = *src++;
            *dst++ = next;
        } while (next != term);
    }
}

void Text_AppendFFTerminatedBytes(u8 *dst, u8 *src)
{
    int scan;
    u8 first;

    scan = *dst++;
    if (scan != 0xFF) {
        int term;
        int next;

        term = 0xFF;
        do {
            next = *dst++;
        } while (next != term);
    }

    dst--;

    first = *src++;
    *dst++ = first;
    if (first != 0xFF) {
        int term;
        int next;

        term = 0xFF;
        do {
            next = *src++;
            *dst++ = next;
        } while (next != term);
    }
}

void Inv_BuildItemGridFromCategory(void)
{
    int i;
    s16 *clear;
    ItemDataRecord *src;
    int category;
    int base;
    int lookup;

    Inv_SelectActiveList(0);

    i = 0x31;
    clear = &g_AyaInventoryItems[0x31];
    do {
        *clear = 0;
        i--;
        clear--;
    } while (i >= 0);

    i = 0;
    category = 0x13;
    do {
        lookup = i;
        i++;
        src = Item_LookupBaseData(lookup);
    } while (src != 0 && src->kind != category);

    g_InvCategoryBaseItemId = i;

    /* Nine grid cells cycle through the three records after the first
     * category match; each cell clears its third base stat and sets the
     * third bonus to the 999 placeholder. */
    i = 0;
    do {
        base = g_InvCategoryBaseItemId + i % 3;
        src = Item_LookupBaseData(base - 1);
        g_InvSpecialItemRecords[i] = *src;
        g_InvSpecialItemRecords[i].baseStats[2] = 0;
        g_InvSpecialItemRecords[i].bonusStats[2] = 999;
    } while (++i < 9);

    i = 0x63;
    clear = &g_WayneStorageItems[0x63];
    do {
        *clear = 0;
        i--;
        clear--;
    } while (i >= 0);

    i = 0x51;
    clear = (s16 *)&g_InvItemArray[0x51];
    do {
        *clear = 0;
        i--;
        clear--;
    } while (i >= 0);

    g_InvActiveListOverride = 0;
}

void Inv_SelectActiveList(int useOverride) {
    if (useOverride != 0 && g_InvActiveListOverride != 0) {
        g_InvItemPtr = g_InvActiveListOverride;
        g_InvSelectionBits = g_InvStorageSelectionBits;
        g_InvSelectionBitWords = 4;
        g_InvSlotLimit = g_InvOverrideSlotLimit;
    } else {
        g_InvItemPtr = g_AyaInventoryItems;
        g_InvSlotLimit = Inv_GetAyaSlotLimit();
        g_InvSelectionBits = g_InvSelectionBitStorage;
        g_InvSelectionBitWords = 2;
    }
}

void Inv_SetActiveListOverride(s16 *items, int count) {
    g_InvActiveListOverride = items;
    g_InvOverrideSlotLimit = count;
}

void Inv_ResetActiveList(void) {
    g_InvActiveListOverride = 0;
    g_InvOverrideSlotLimit = 0;
    g_InvItemPtr = g_AyaInventoryItems;
    g_InvSlotLimit = Inv_GetAyaSlotLimit();
    g_InvSelectionBits = g_InvSelectionBitStorage;
    g_InvSelectionBitWords = 2;
}

int Inv_IsActiveListOverrideSelected(void) {
    return g_InvItemPtr != g_AyaInventoryItems;
}

void Inv_SetAyaSlotCount(int count) {
    if (count >= 0x33) {
        count = 0x32;
    }

    g_AyaInventorySlotCount[0] = count;
    if (g_InvItemPtr == (s16 *)(g_AyaInventorySlotCount + 0x3C)) {
        g_InvSlotLimit = Inv_GetAyaSlotLimit();
    }
}

int Inv_GetAyaSlotLimit(void) {
    if (g_AyaInventorySlotCount[0] + Inv_GetBonusSlotCount() >= 0x33) {
        return 0x32;
    }

    return g_AyaInventorySlotCount[0] + Inv_GetBonusSlotCount();
}

void Inv_ClearEquipFlagForKind(InvItemSlot *obj) {
    u8 *base;
    u8 *entry;
    u8 *end;
    int kind;
    int is_kind_9;
    int kind_9;
    int one;

    if (obj != 0) {
        kind = obj->kind;
        base = (u8 *)g_InvItemSlotArray;
        kind ^= 9;
        is_kind_9 = (unsigned int)kind < 1;
        kind = (int)(base + 0x1000);
        if (base < (u8 *)kind) {
            kind_9 = 9;
            one = 1;
            entry = base + 5;
            end = base + 0x1005;

            do {
                if (entry[1] != kind_9) {
                    if (is_kind_9 == one) {
                        entry += 0x20;
                        continue;
                    }
                } else if (is_kind_9 == 0) {
                    entry += 0x20;
                    continue;
                }

                entry[0] &= 0xEF;
                entry += 0x20;
            } while (entry < end);
        }

        obj->flags |= 0x10;
    }
}

#define NULL ((void *)0)

void *Inv_LookupActiveListDisplayData(s32 index);

void *Inv_LookupActiveListDisplayData(s32 index) {
    s16 itemId;
    u32 lookupIndex;
    ItemDataRecord *entry;
    void *displayData;

    itemId = g_InvItemPtr[index];
    displayData = NULL;
    if ((itemId - 0x100) < 0x80U) {
        entry = &((ItemDataRecord *)g_EquipItemDataTable)[itemId];
        if (entry->flags & 0x10) {
            displayData = g_EquipItemDataTable + 0x31F8;
            if (entry->kind == 9) {
                displayData = g_EquipItemDataTable + 0x3208;
            }
        } else {
            lookupIndex = entry->itemId - 1;
            displayData = Text_LookupTable8(lookupIndex);
        }
    } else {
        lookupIndex = itemId - 1;
        if (lookupIndex >= 0xFFU) {
            if ((itemId - 0x200) < 9U) {
                lookupIndex = (g_InvCategoryBaseItemId + itemId) - 0x201;
                displayData = Text_LookupTable8(lookupIndex);
            }
        } else {
            displayData = Text_LookupTable8(lookupIndex);
        }
    }
    return displayData;
}
#undef NULL

/* Inventory item-ID and active-slot lookup helpers. */

ItemDataRecord *Inv_LookupData(int arg0) {
    if ((arg0 - 0x100U) < 0x80U) {
        return &g_InventoryRuntime.equipment[arg0 - 0x100];
    }

    if ((arg0 - 1U) < 0xFFU) {
        return Item_LookupBaseData(arg0 - 1);
    }

    if ((arg0 - 0x200U) >= 9U) {
        return 0;
    }

    return (ItemDataRecord *)&g_KeyItemDataTable[arg0 << 5];
}

ItemDataRecord *Inv_LookupActiveListData(int index) {
    int value;
    int saved_value;
    ItemDataRecord *entry;

    if (index >= 0 && index < g_InvSlotLimit) {
        value = g_InvItemPtr[index];
        saved_value = value;
        if (value - 0x100U < 0x80U) {
            entry = &g_InventoryRuntime.equipment[value - 0x100];
        } else if (value - 1U < 0xFFU) {
            entry = Item_LookupBaseData(value - 1);
        } else if (saved_value - 0x200U < 9U) {
            entry = (ItemDataRecord *)(g_KeyItemDataTable + (saved_value << 5));
        } else {
            entry = 0;
        }
    } else {
        entry = 0;
    }

    return entry;
}

int Inv_FindIndexByData(void *needle) {
    int limit;
    int index;
    int item_id;
    int offset;
    void *data;
    int saved_item_id;
    int stack_pad[2];

    limit = g_InvSlotLimit;
    index = 0;
    if (limit > 0) {
        do {
            if (index < 0 || index >= limit) {
                data = 0;
            } else {
                item_id = g_InvItemPtr[index];
                saved_item_id = (s16)item_id;
                if (item_id - 0x100U < 0x80U) {
                    data = &g_InventoryRuntime.equipment[item_id - 0x100];
                } else {
                    offset = item_id - 1;
                    if ((unsigned int)offset < 0xFF) {
                        data = Item_LookupBaseData(offset);
                    } else if (saved_item_id - 0x200U < 9U) {
                        int shifted;

                        shifted = saved_item_id << 5;
                        data = g_KeyItemDataTable + shifted;
                    } else {
                        data = 0;
                    }
                }
            }

            if (data == needle) {
                break;
            }
            limit = g_InvSlotLimit;
            index++;
        } while (index < limit);
    }

    {
        int result;
        result = -1;
        if (index < g_InvSlotLimit) {
            result = index;
        }
        return result;
    }
}

short Inv_GetActiveListItem(int index) {
    return g_InvItemPtr[index];
}

void Menu_DrawSlotRow(ItemDataRecord *entry, u8 *text) {
    int type;
    int sprite;
    int ammo;

    Draw_StatePush();

    if (entry->kind < 8) {
        sprite = entry->baseStats[2] + entry->bonusStats[2];
        ammo = entry->ammo;
        if (sprite < 1000) {
            if (ammo == sprite) {
                Draw_AllocSprite(0x69);
            }
        } else if (ammo == 999) {
            Draw_AllocSprite(0x69);
        }
    }

    Draw_AllocSprite(entry->iconId - 1);
    Draw_OffsetCursor(0x10, 0);

    if (text != 0) {
        Draw_PrintRawText(text);
    }

    type = entry->kind;
    if (type != 0 && (unsigned int)type < 8) {
        if (type - 4 > 0) {
            if (type == 4) {
                Draw_StatePop();
                return;
            }
        }
    } else {
        int cmp;
        if ((unsigned int)type < 0x13 || (cmp = type - 0x12) == 0) {
            Draw_StatePop();
            return;
        }
    }

    Draw_OffsetCursor(0x58, 0);

    type = entry->kind;
    if (type != 0 && (unsigned int)type < 8) {
        if (type - 4 > 0) {
            sprite = type + 0x1A;
        } else {
            sprite = 0x1F;
        }
    } else if ((unsigned int)type < 0x13) {
        sprite = 0x1E;
    } else {
        sprite = type + 0xC;
    }

    Draw_AllocSprite(sprite);
    Draw_OffsetCursor(0, 5);
    Draw_PrintNumberWidth3Unk(entry->ammo);

    Draw_StatePop();
}

void Menu_CursorRenderData(ItemDataRecord *record) {
    u8 *cursor;

    if (record->flags & ITEM_DATA_FLAG_GENERIC_DESCRIPTION) {
        cursor = g_CursorRenderMetadataWindows[0].text;
        if (record->kind == ITEM_KIND_ARMOR) {
            cursor = g_CursorRenderMetadataWindows[1].text;
        }
    } else {
        cursor = Text_LookupTable8(record->itemId - 1);
    }

    Menu_DrawSlotRow(record, cursor);
}

#define NULL ((void *)0)

void Menu_DrawActiveListSlot(s32 arg0) {
    s16 temp_v1;
    s32 temp_v1_3;
    s32 temp_a1;
    u32 temp_a0;
    u32 var_a0;
    ItemDataRecord *temp_v1_2;
    u8 *var_a1;
    ItemDataRecord *var_v0;
    u8 *saved_a1;

    temp_v1 = g_InvItemPtr[arg0];
    var_a1 = NULL;
    if ((temp_v1 - 0x100) < 0x80U) {
        temp_v1_2 = &((ItemDataRecord *)g_EquipItemDataTable)[temp_v1];
        if (temp_v1_2->flags & 0x10) {
            var_a1 = g_EquipItemDataTable + 0x31F8;
            if (temp_v1_2->kind == 9) {
                var_a1 = g_EquipItemDataTable + 0x3208;
            }
        } else {
            var_a0 = temp_v1_2->itemId - 1;
            var_a1 = Text_LookupTable8(var_a0);
        }
    } else {
        var_a0 = temp_v1 - 1;
        if (var_a0 >= 0xFFU) {
            if ((temp_v1 - 0x200) < 9U) {
                var_a0 = (g_InvCategoryBaseItemId + temp_v1) - 0x201;
                var_a1 = Text_LookupTable8(var_a0);
            }
        } else {
            var_a1 = Text_LookupTable8(var_a0);
        }
    }
    saved_a1 = var_a1;
    if ((arg0 >= 0) && (arg0 < g_InvSlotLimit)) {
        temp_v1_3 = g_InvItemPtr[arg0];
        /* Inventory IDs retain their signed 16-bit representation. */
        temp_a1 = (s16)temp_v1_3;
        if ((temp_v1_3 - 0x100) < 0x80U) {
            var_v0 = &((ItemDataRecord *)g_EquipItemDataTable)[temp_v1_3];
        } else {
            temp_a0 = temp_v1_3 - 1;
            if (temp_a0 < 0xFFU) {
                var_v0 = Item_LookupBaseData(temp_a0);
            } else if ((temp_a1 - 0x200) < 9U) {
                temp_v1_3 = temp_a1 << 5;
                var_v0 = (ItemDataRecord *)(temp_v1_3 + g_KeyItemDataTable);
            } else {
                var_v0 = NULL;
            }
        }
    } else {
        var_v0 = NULL;
    }
    if (var_v0 != NULL) {
        Menu_DrawSlotRow(var_v0, saved_a1);
    }
}
#undef NULL

/* Find a run of empty slots, after selecting Aya's inventory. The two
 * register bindings keep the scan limits in their retail registers and
 * are recorded in debt. No barriers or instruction assembly are needed. */
int Inv_FindFirstEmptySlot(int count) {
    short *p, *base;
    register short *end asm("$3");
    int n;

    g_InvItemPtr = g_AyaInventoryItems;
    g_InvSlotLimit = Inv_GetAyaSlotLimit();
    g_InvSelectionBits = g_InvSelectionBitStorage;
    g_InvSelectionBitWords = 2;
    p = g_InvItemPtr;
    end = p + g_InvSlotLimit - count;
    if (p < end + 1) {
        base = p;
        {
            register short *limit asm("$6") = end;
            do {
                register short *bound = limit + 1;
                while (1) {
                    if (!*p) break;
                    p++;
                    if (p >= bound) break;
                }
                if (p < limit + 1) {
                    for (n = 1; n < count && p[n] == 0; n++) {
                    }
                    if (n >= count) return p - base;
                }
                p++;
            } while (p < limit + 1);
        }
    }
    return -1;
}

static inline int FindFreeEquipmentRecord(void) {
    int result;
    ItemDataRecord *p = g_InventoryRuntime.equipment;
    ItemDataRecord *end = p + 128;

    if (p < end) {
        do {
            if (!p->iconId)
                break;
            p++;
        } while (p < end);
        if (p < g_InventoryRuntime.equipment + 128) {
            int index = p - g_InventoryRuntime.equipment;
            result = index;
        } else {
            result = -1;
        }
    } else {
        result = -1;
    }
    return result;
}

static inline int FindFreeActiveSlot(void) {
    s16 *p = g_InvItemPtr, *end = p + g_InvSlotLimit;

    if (p < end) {
        while (p < end && *p != 0)
            p++;
        if (p < g_InvItemPtr + g_InvSlotLimit)
            return p - g_InvItemPtr;
    }
    return -1;
}

ItemDataRecord *Inv_FindSlotByIndex(int id) {
    ItemDataRecord *result = 0;
    int record = FindFreeEquipmentRecord();
    int slot = FindFreeActiveSlot();

    if (record >= 0 && slot >= 0) {
        result = &g_InventoryRuntime.equipment[record];
        *result = *Item_LookupBaseData(id - 1);
        g_InvItemPtr = g_InventoryRuntime.slots;
        g_InvSlotLimit = Inv_GetAyaSlotLimit();
        g_InvSelectionBits = g_InvSelectionBitStorage;
        g_InvSelectionBitWords = 2;
        g_InvItemPtr[slot] = record + 256;
    }
    return result;
}

static inline int FindActiveSlot(int value) {
    s16 *p = g_InvItemPtr, *end = p + g_InvSlotLimit;
    if (p < end) {
        while (p < end && *p != value)
            p++;
        if (p < g_InvItemPtr + g_InvSlotLimit)
            return p - g_InvItemPtr;
    }
    return -1;
}
static inline void ClampAmmo(ItemDataRecord *item) {
    int current = item->ammo;
    if (item->baseStats[2] + item->bonusStats[2] < 1000 ?
        item->baseStats[2] + item->bonusStats[2] < current : 999 < current) {
        item->ammo = item->baseStats[2] + item->bonusStats[2] < 1000 ?
            item->baseStats[2] + (u16)item->bonusStats[2] : 999;
    }
}

/* Add loaded ammunition even when no empty inventory slot is available. */
int Inv_WriteSlotById(ItemDataRecord *item) {
    unsigned kind = item->kind;
    int failed = 0;
    int category;

    if (kind < 16) {
        if (kind && kind < 8)
            category = (int)(kind - 4) > 0 ? kind - 5 : 0;
        else
            category = kind >= 19 ? kind - 19 : -1;
    } else {
        category = kind - 16;
    }
    if ((unsigned)category < 3) {
        ItemDataRecord *pool;
        if (FindActiveSlot(category + 512) < 0) {
            int slot = FindFreeActiveSlot();
            if (slot >= 0)
                g_InvItemPtr[slot] = category + 512;
            else
                failed = 1;
        }
        pool = &g_InvSpecialItemRecords[category];
        pool->ammo += item->ammo;
        ClampAmmo(pool);
    } else {
        failed = 1;
    }
    return failed;
}

/* Insert an item into a free active slot. Non-zero means the insert failed. */
#define NULL ((void *)0)
s32 Inv_AddItem(s32 arg0) {
    s32 ret;
    s32 slot;
    s32 t;
    s16 *cur;
    s16 *end;
    ItemDataRecord *ent;

    ret = 0;
    cur = g_InvItemPtr;
    end = g_InvItemPtr + g_InvSlotLimit;
    if (cur < end) {
        do {
            if (*cur == 0) {
                break;
            }
            cur++;
        } while (cur < end);
        if (cur < g_InvItemPtr + g_InvSlotLimit) {
            t = cur - g_InvItemPtr;
        } else {
            t = -1;
        }
    } else {
        t = -1;
    }
    slot = t;
    if (arg0 < 0x100) {
        ent = Item_LookupBaseData(arg0 - 1);
        if (ent == NULL) {
            return ret;
        }
        switch (ent->kind) {
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
            ret = Inv_FindSlotByIndex(arg0) == 0;
            break;
        case 10:
        case 12:
        case 13:
        case 14:
        case 15:
            if (slot >= 0) {
                g_InvItemPtr[slot] = arg0;
            } else {
                ret = 1;
            }
            break;
        case 16:
        case 17:
        case 18:
            ret = Inv_WriteSlotById(ent);
            break;
        }
    } else {
        if (slot >= 0) {
            g_InvItemPtr[slot] = arg0;
        } else {
            ret = 1;
        }
    }
    return ret;
}
#undef NULL

int Inv_CountByValue(int arg0) {
    int count;
    u16 *ptr;
    register u16 *end;
    register u16 *iter_end;
    int temp;
    int value;

    count = 0;
    if ((arg0 >= g_InvCategoryBaseItemId) && (arg0 < g_InvCategoryBaseItemId + 3)) {
        count = g_InvCategoryItemTable[arg0 - g_InvCategoryBaseItemId].count;
    } else {
        ptr = g_InvItemPtr;
        end = ptr + g_InvSlotLimit;
        if (ptr < end) {
            iter_end = end;
            do {
                value = *ptr;
                if (value - 0x100U < 0x80U) {
                    temp = g_ItemCodeRemapTable[(value << 16) >> 11];
                } else {
                    temp = (value << 16) >> 16;
                }
                count += ((temp ^ arg0) < 1U);
                ptr++;
            } while (ptr < iter_end);
        }
    }

    return count;
}

/* Contiguous inventory counts, removal-by-ID, active-item kind lookup, base
 * item kinds, the equipped-slot reservation test and the packed-list
 * accessors. */

int Inv_CountByCategory(int arg0) {
    int index;
    int count;

    index = 0;
    count = 0;
    while (index < g_InvSlotLimit) {
        if (Inv_GetActiveListItemType(index) == arg0) {
            count++;
        }
        index++;
    }

    return count;
}

int Inv_CountWayneStorageItemType(int arg0) {
    short *item;
    int index;
    int count;
    int itemId;

    count = 0;
    index = 0;
    item = g_WayneStorageItems;
    do {
        itemId = *item;
        if (itemId != 0) {
            if (Item_LookupBaseData(itemId - 1)->kind == arg0) {
                count++;
            }
        }
        index++;
        item++;
    } while (index < 0x64);

    return count;
}

int Inv_CountTotal(void) {
    short *item;
    short *end;
    int count;

    g_InvItemPtr = g_AyaInventoryItems;
    count = 0;
    g_InvSlotLimit = Inv_GetAyaSlotLimit();
    g_InvSelectionBits = g_InvSelectionBitStorage;
    g_InvSelectionBitWords = 2;

    item = g_InvItemPtr;
    end = item + g_InvSlotLimit;
    while (item < end) {
        count += *item != 0;
        item++;
    }

    return count;
}

int Inv_FindItemById(int arg0) {
    short *item;
    short *end;
    int result;

    g_InvItemPtr = g_AyaInventoryItems;
    g_InvSlotLimit = Inv_GetAyaSlotLimit();
    g_InvSelectionBits = g_InvSelectionBitStorage;
    g_InvSelectionBitWords = 2;

    item = g_InvItemPtr;
    end = item + g_InvSlotLimit;

    if (item < end) {
        while (*item != arg0) {
            item++;
            if (item >= end) {
                break;
            }
        }
        if (item < g_InvItemPtr + g_InvSlotLimit) {
            result = item - g_InvItemPtr;
        } else {
            result = -1;
        }
    } else {
        result = -1;
    }

    if (result >= 0) {
        Inv_RemoveActiveListItem(result);
        return 0;
    }
    return result;
}

int Inv_GetActiveListItemType(int index)
{
    int value;
    int saved_value;
    ItemDataRecord *entry;

    if (index >= 0 && index < g_InvSlotLimit) {
        value = g_InvItemPtr[index];
        saved_value = value;
        if (value - 0x100U < 0x80U) {
            entry = (ItemDataRecord *)(g_EquipItemDataTable + (value << 5));
        } else if (value - 1U < 0xFFU) {
            entry = Item_LookupBaseData(value - 1);
        } else if (saved_value - 0x200U < 9U) {
            entry = (ItemDataRecord *)(g_KeyItemDataTable + (saved_value << 5));
        } else {
            entry = 0;
        }
    } else {
        entry = 0;
    }

    if (entry == 0) {
        return 0;
    }
    return entry->kind;
}

int Item_GetBaseType(int index) {
    return Item_LookupBaseData(index - 1)->kind;
}

int Inv_IsAyaInventorySlotReserved(int arg0) {
    int result = 0;

    if (g_InvItemPtr == g_AyaInventoryItems) {
        if (g_AyaEquippedWeaponSlot[0] == arg0 || g_AyaEquippedArmorSlot[0] == arg0) {
            result = 1;
        }
    }

    return result;
}

int Inv_GetPackedListCount(void) {
    return g_InvPackedListCount;
}

int Inv_GetPackedListCursor(void) {
    return g_InvPackedListCursor;
}

void Inv_BuildFilteredPackedList(int mask) {
    int limit;
    int index;
    int item_id;
    int offset;
    ItemDataRecord *data;
    int type;
    s16 *out;
    int saved_item_id;
    int stack_pad[2];

    limit = g_InvSlotLimit;
    out = g_InvPackedList;
    index = 0;
    if (limit > 0) {
        do {
            if (index < 0 || index >= limit) {
                data = 0;
            } else {
                item_id = g_InvItemPtr[index];
                saved_item_id = (s16)item_id;
                if (item_id - 0x100U < 0x80U) {
                    data = (ItemDataRecord *)(g_EquipItemDataTable +
                                              (item_id << 5));
                } else {
                    offset = item_id - 1;
                    if ((unsigned int)offset < 0xFF) {
                        data = Item_LookupBaseData(offset);
                    } else if (saved_item_id - 0x200U < 9U) {
                        int shifted;

                        shifted = saved_item_id << 5;
                        data = (ItemDataRecord *)(g_KeyItemDataTable + shifted);
                    } else {
                        data = 0;
                    }
                }
            }

            if (data != 0) {
                type = data->kind;
            } else {
                type = 0;
            }
            if (((mask >> type) & 1) != 0) {
                *out++ = index;
            }

            limit = g_InvSlotLimit;
            index++;
        } while (index < limit);
    }

    g_InvPackedListCursor = 0;
    g_InvPackedListCount = (out - g_InvPackedList);

}

void Inv_BuildFilteredPackedListExcluding(int mask, int excluded) {
    int limit;
    int index;
    int item_id;
    int offset;
    ItemDataRecord *lookup;
    ItemDataRecord *data;
    int type;
    s16 *out;
    int saved_item_id;
    int stack_pad[2];

    limit = g_InvSlotLimit;
    out = g_InvPackedList;
    index = 0;
    if (limit > 0) {
        do {
            if (index >= 0 && index < limit) {
                item_id = g_InvItemPtr[index];
                saved_item_id = item_id;
                if (item_id - 0x100U < 0x80U) {
                    lookup = (ItemDataRecord *)(g_EquipItemDataTable +
                                               (item_id << 5));
                } else {
                    offset = item_id - 1;
                    if ((unsigned int)offset < 0xFF) {
                        lookup = Item_LookupBaseData(offset);
                    } else if (saved_item_id - 0x200U < 9U) {
                        int shifted;
                        shifted = saved_item_id << 5;
                        lookup = (ItemDataRecord *)(g_KeyItemDataTable + shifted);
                    } else {
                        lookup = 0;
                    }
                }
                data = lookup;
            } else {
                data = 0;
            }
            if ((data != 0) &&
                    (((mask >> data->kind) & 1) != 0) &&
                    (data->tailCount != 0) &&
                    (index != excluded)) {
                *out++ = index;
            }

            limit = g_InvSlotLimit;
            index++;
        } while (index < limit);
    }

    g_InvPackedListCursor = 0;
    g_InvPackedListCount = (out - g_InvPackedList);

}

static inline ItemDataRecord *LookupActiveItem(int index) {
    if (index >= 0 && index < g_InvSlotLimit) return Inv_LookupItemRecord(g_InvItemPtr[index]);
    return 0;
}
static inline void RestoreList(int storage) {
    if (storage && g_InvActiveListOverride != 0) {
        g_InvItemPtr = g_InvActiveListOverride;
        g_InvSelectionBits = g_InvStorageSelectionBits;
        g_InvSelectionBitWords = 4;
        g_InvSlotLimit = g_InvOverrideSlotLimit;
    } else {
        g_InvItemPtr = g_AyaInventoryItems;
        g_InvSlotLimit = Inv_GetAyaSlotLimit();
        g_InvSelectionBits = g_InvSelectionBitStorage;
        g_InvSelectionBitWords = 2;
    }
}

static inline int ListCount(void) {
    if (g_InvActiveListOverride) return 2;
    return 1;
}

static inline int Eligible(int index,int mask) {
    ItemDataRecord *item;
    if (index >= 0 && index < g_InvSlotLimit) item=Inv_LookupItemRecord(g_InvItemPtr[index]);
    else item=0;
    return item && ((mask >> item->kind) & 1) && item->tailCount != 0;
}
int Inv_SetupSlotDisplay(int mask) {
    int i=0, count=0;
    int wasStorage=g_InvItemPtr != g_AyaInventoryItems;
    RestoreList(0);
    for (; i<g_InvSlotLimit; ++i) count += Eligible(i,mask);
    if (ListCount() >= 2 && !(mask & 0x100)) {
        RestoreList(1);
        for (i=0; i<g_InvSlotLimit; ++i) count += Eligible(i,mask);
    }
    RestoreList(wasStorage);
    return count>=2;
}

static inline int RandomByte(void) {
    int i = 0;
    if (++g_ItemDropRngIndex >= 521) {
        for (; i < 32; ++i)
            g_ItemDropRngTable[i] ^= g_ItemDropRngTable[i + 489];
        for (i = 32; i < 521; ++i)
            g_ItemDropRngTable[i] ^= g_ItemDropRngTable[i - 32];
        g_ItemDropRngIndex = 0;
    }
    return g_ItemDropRngTable[g_ItemDropRngIndex];
}

static inline int IsTracked(int index) {
    int result = 0;
    if (g_InvItemPtr == g_AyaInventoryItems) {
        if (g_InventoryRuntime.tracked[0] == index || g_InventoryRuntime.tracked[2] == index)
            result = 1;
    }
    return result;
}
int Inv_PickRandomItem(int unused) {
    s16 *out = g_InvPackedList;
    int i;
    ItemDataRecord *item;
    for (i = 0; i < g_InvSlotLimit; ++i) {
        item = LookupActiveItem(i);
        if (item && (item->flags & 128) && !IsTracked(i)) *out++ = i;
    }
    g_InvPackedListCursor = 0;
    g_InvPackedListCount = out - g_InvPackedList;
    if (!g_InvPackedListCount) return 0;
    return g_InvItemPtr[g_InvPackedList[(g_InvPackedListCount * RandomByte()) >> 8]];
}

/* Historical name: formats the item message into the shared FF-terminated buffer. */
static inline void CopyText(u8 *dest, const u8 *src) {
    while ((*dest++ = *src++) != 255) {}
}
static inline void AppendText(u8 *dest, const u8 *src) {
    while (*dest++ != 255) {}
    --dest;
    while ((*dest++ = *src++) != 255) {}
}
u8 *Inv_GetItemEffectData(int item, int mode) {
    if (!item) CopyText(g_InvItemMessageBuffer, Text_LookupTable4(19));
    else if (mode >= 2) CopyText(g_InvItemMessageBuffer, Text_LookupTable4(18));
    else if (!g_SaveMetadataWindowIndex) {
        CopyText(g_InvItemMessageBuffer, Text_LookupTable8(item - 1));
        AppendText(g_InvItemMessageBuffer, Text_LookupTable4(mode + 16));
    } else {
        CopyText(g_InvItemMessageBuffer, Text_LookupTable4(mode + 16));
        AppendText(g_InvItemMessageBuffer, Text_LookupTable8(item - 1));
    }
    return g_InvItemMessageBuffer;
}

/* First empty slot of the active list, or -1. */
static inline int FindFreeSlot(void) {
    s16 *p = g_InvItemPtr, *end = p + g_InvSlotLimit;
    int slot;

    if (p < end) {
        do {
            if (!*p) break;
            p++;
        } while (p < end);
        if (p < g_InvItemPtr + g_InvSlotLimit) {
            slot = p - g_InvItemPtr;
        } else {
            slot = -1;
        }
    } else {
        slot = -1;
    }
    return slot;
}

/* JP compacts the list right away instead of leaving the count in
 * g_InvPendingCompactCount for Inv_CompactActiveListSlots. */
int Inv_CheckFreeSlotCapacity(int requested_slots) {
    s16 *slot;
    s16 *end;
    int capacity;
    int capacity_before_scan;
    int bonus_slots;
    int used_slots;
    int occupied;
    int result;
    int index, limit, from;

    g_InvItemPtr = g_AyaInventoryItems;
    g_InvSlotLimit = Inv_GetAyaSlotLimit();
    g_InvSelectionBits = g_InvSelectionBitStorage;
    g_InvSelectionBitWords = 2;

    bonus_slots = Inv_GetBonusSlotCount();
    if (g_InvBaseCapacity + bonus_slots < 51) {
        used_slots = 0;
        capacity_before_scan =
            g_InvBaseCapacity + Inv_GetBonusSlotCount();
    } else {
        used_slots = 0;
        capacity_before_scan = 50;
    }

    g_InvItemPtr = g_AyaInventoryItems;
    capacity = capacity_before_scan;
    g_InvSlotLimit = Inv_GetAyaSlotLimit();
    g_InvSelectionBits = g_InvSelectionBitStorage;
    g_InvSelectionBitWords = 2;

    slot = g_InvItemPtr;
    end = slot + g_InvSlotLimit;
    while (slot < end) {
        occupied = *slot++;
        occupied = occupied != 0;
        used_slots += occupied;
    }

    result = requested_slots <= capacity - used_slots;
    if (result == 1) {
        while (requested_slots > 0) {
            index = FindFreeSlot();
            if (index >= 0) {
                if (g_InvBaseCapacity + Inv_GetBonusSlotCount() < 51) {
                    limit = g_InvBaseCapacity + Inv_GetBonusSlotCount();
                } else {
                    limit = 50;
                }
                from = limit - requested_slots;
                g_InvItemPtr[index] = g_InvItemPtr[from];
                g_InvItemPtr[from] = 0;
                if (g_InvTrackedSlots[0] == from) {
                    g_InvTrackedSlots[0] = index;
                } else if (g_InvTrackedSlots[2] == from) {
                    g_InvTrackedSlots[2] = index;
                }
            }
            requested_slots--;
        }
    }
    return result;
}

int Inv_CheckItemEquippable(unsigned int list_index, int modifier_index) {
    /* Match the retail 0x60-byte stack frame; remove with a cleaner TU model. */
    unsigned char stack_pad[32];
    int selected_a;
    int selected_b;
    ItemDataRecord *item_a;
    ItemDataRecord *item_b;
    int item_id;
    int count;
    int i;
    int result;
    int mask;
    int modifier;
    int existing;
    int different_slot;

    mask = 0;
    different_slot = 0;
    selected_a = Inv_RestoreSelection(list_index);
    item_a = 0;
    if (selected_a >= 0 && selected_a < g_InvSlotLimit) {
        ItemDataRecord *resolved;
        int saved_id;
        int scaled_id;
        item_id = g_InvItemPtr[selected_a];
        saved_id = (s16)item_id;
        if (item_id - 0x100U < 0x80U) {
            resolved = (ItemDataRecord *)(g_EquipItemDataTable + (item_id << 5));
        } else if (item_id - 1U < 0xFFU) {
            resolved = Item_LookupBaseData(item_id - 1);
        } else if (saved_id - 0x200U < 9U) {
            resolved = &((ItemDataRecord *)g_KeyItemDataTable)[saved_id];
        } else {
            resolved = 0;
        }
        item_a = resolved;
    }

    selected_b = Inv_RestoreSelection(list_index < 1);
    item_b = 0;
    if (selected_b >= 0 && selected_b < g_InvSlotLimit) {
        ItemDataRecord *resolved;
        int saved_id;
        int scaled_id;
        item_id = g_InvItemPtr[selected_b];
        saved_id = (s16)item_id;
        if (item_id - 0x100U < 0x80U) {
            scaled_id = item_id << 5;
            resolved = (ItemDataRecord *)(g_EquipItemDataTable + scaled_id);
        } else if (item_id - 1U < 0xFFU) {
            resolved = Item_LookupBaseData(item_id - 1);
        } else if (saved_id - 0x200U < 9U) {
            resolved = &((ItemDataRecord *)g_KeyItemDataTable)[saved_id];
        } else {
            resolved = 0;
        }
        item_b = resolved;
    }

    if (modifier_index >= 0) {
        count = item_b->tailCount;
        modifier = item_a->tailData[modifier_index];
        i = 0;
        if (count > 0) {
            while (1) {
                if (item_b->tailData[i] == modifier) {
                    break;
                }
                ++i;
                if (i >= count) {
                    break;
                }
            }
            if (i < item_b->tailCount) return 4;
        }

        for (i = 0; i < item_b->tailCount; i++) {
            if (item_b->tailData[i] == 0) {
                break;
            }
        }

        if (i >= item_b->tailCount) {
            modifier = item_a->tailData[modifier_index];
            modifier &= 0xE0;
            if (modifier == 0) {
                return 3;
            }

            {
                int third_count;
                int third_modifier;
                third_count = item_b->tailCount;
                if (third_count > 0) {
                    i = 0;
                    third_modifier = modifier;
                    /* The modifier kind is saved; reuse its scratch for the bound. */
                    modifier = third_count;
                    while (1) {
                        if ((item_b->tailData[i] & 0xE0) == third_modifier) {
                            break;
                        }
                        ++i;
                        if (i >= modifier) {
                            break;
                        }
                    }
                    if (i >= item_b->tailCount) return 3;
                } else {
                    return 3;
                }
            }
        }
    }

    Inv_RestoreSelection(list_index);
    if (g_InvItemPtr == g_AyaInventoryItems && selected_a == g_AyaEquippedArmorSlot[0]) {
        int raw_count;
        raw_count = item_a->tailCount;
        i = 0;
        if (raw_count > 0) {
            /* The modifier scratch is free again; it holds the bound. */
            modifier = raw_count;
            while (1) {
                raw_count = item_a->tailData[i];
                raw_count &= 0x1F;
                existing = raw_count - 8;
                if ((unsigned int)existing < 3) {
                    break;
                }
                ++i;
                if (i >= modifier) {
                    break;
                }
            }
            if (i < item_a->tailCount) {
                mask = 1 << existing;
                different_slot = i != modifier_index;
            }
        }
    } else {
        Inv_RestoreSelection(list_index < 1);
        if (g_InvItemPtr == g_AyaInventoryItems) {
            if (selected_b == g_AyaEquippedArmorSlot[0] && modifier_index >= 0) {
                int raw;
                int final_count;
                int final_existing;
                raw = item_a->tailData[modifier_index];
                raw &= 0x1F;
                /* The earlier scan is finished; reuse its count for the requested slot. */
                count = raw - 8;
                if ((unsigned int)count < 3) {
                    raw = item_b->tailCount;
                    i = 0;
                    if (raw > 0) {
                        final_count = raw;
                        while (1) {
                            raw = item_b->tailData[i];
                            raw &= 0x1F;
                            final_existing = raw - 8;
                            if ((unsigned int)final_existing < 3) {
                                break;
                            }
                            ++i;
                            if (i >= final_count) {
                                break;
                            }
                        }
                        if (i < item_b->tailCount) {
                            mask = (1 << count) - (1 << final_existing);
                        }
                    }
                }
            } else {
                return 1;
            }
        } else {
            return 1;
        }
    }

    result = Inv_CheckFreeSlotCapacity(mask);
    if (result != 1 || !different_slot) {
        return result;
    }
    return 2;
}
