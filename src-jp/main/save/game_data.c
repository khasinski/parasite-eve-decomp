/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
/* MASPSX_FORCE_G0: 1 */

#include "common.h"
#include "pe1/aya.h"
#include "pe1/draw_state.h"
#include "pe1/draw_wipe_rect.h"
#include "pe1/game_state.h"
#include "pe1/game_timers.h"
#include "pe1/inventory_category.h"
#include "pe1/inventory_slots.h"
#include "pe1/inventory_state.h"
#include "pe1/memcard.h"
#include "pe1/menu_background.h"
#include "pe1/menu_inventory.h"
#include "pe1/menu_queue.h"
#include "pe1/menu_sound.h"
#include "pe1/menu_state.h"
#include "pe1/menu_widget.h"
#include "pe1/psyq_bios.h"
#include "pe1/save.h"
#include "pe1/scene_dispatch.h"
#include "pe1/scene_room.h"
#include "pe1/text.h"
#include "include_asm.h"

/* COMMON supplies small-data metadata to MASPSX: these words are reached
 * through $gp, every other extern stays absolute. */
int g_InvCategoryBaseItemId;
s32 g_InvPackedListCount;
s16 *g_InvItemPtr;
s32 g_InvSlotLimit;
u32 *g_InvSelectionBits;
s32 g_InvSelectionBitWords;
s32 g_InvPackedListCursor;
s32 g_InvRebuildFilterMode;
s32 g_InvRebuildMinFirstStat;
int g_MenuBattleEquipMode;
u8 *g_SaveMetadataCursor;
ItemDataRecord *g_SaveMetadataSourceRecord;
int g_SaveMetadataWindowIndex;
int g_SaveMetadataWindowLength;
int g_MenuScrollDirectionValueUnwritten;
int g_MemCardDialogState;
int g_MenuScrollStep;

int Aya_StatBinarySearch(int value, int *table) {
    int step;
    int index;
    int result;

    index = 0x40;
    step = 0x20;

    while (step != 0) {
        if (value >= table[index]) {
            index += step;
        } else if (value < table[index - 1]) {
            index -= step;
        } else {
            step = 0;
        }
        step >>= 1;
    }

    index--;
    result = 0x62;
    if (index < 0x63) {
        result = index;
    }

    return result;
}

static inline int InterpolateSubLevel(int value, int *cursor) {
    int current_reg;
    int next_reg;
    int delta_reg;
    int numerator_reg;
    next_reg = cursor[1];
    current_reg = cursor[0];
    if (next_reg != current_reg) {
        delta_reg = value - current_reg;
        numerator_reg = ((delta_reg << 1) + delta_reg) * 16 + delta_reg;
        return numerator_reg / (next_reg - current_reg);
    }
    return 0;
}

void Aya_StatQueryLevelAndSubLevel(int kind, int value, int *level_out, int *sublevel_out) {
    int saved_value;
    int *level_ptr;
    int *sublevel_ptr;
    int *table;
    int index;
    int step;
    int offset;
    int *loop_cursor;
    int *cursor;
    int sublevel;

    saved_value = value;
    level_ptr = level_out;
    sublevel_ptr = sublevel_out;
    table = Aya_GetStatGrowthTable(kind);

    index = 0x40;
    step = 0x20;
    do {
        offset = index << 2;
        loop_cursor = (int *)(offset + (int)table);
        if (saved_value >= loop_cursor[0]) {
            index += step;
        } else if (saved_value < loop_cursor[-1]) {
            index -= step;
        } else {
            step = 0;
        }
        step >>= 1;
    } while (step != 0);

    index--;
    step = 0x62;
    if (index < 0x63) {
        step = index;
    }
    index = step;

    if (index < 0x62) {
        offset = index << 2;
        cursor = (int *)(offset + (int)table);
        sublevel = InterpolateSubLevel(saved_value, cursor);
    } else {
        sublevel = 0;
    }

    if (sublevel >= 0x31) {
        sublevel = 0x30;
    }

    if (level_ptr != 0) {
        *level_ptr = index;
    }
    if (sublevel_ptr != 0) {
        if (index < 0x62) {
            *sublevel_ptr = sublevel;
        } else {
            *sublevel_ptr = 0x30;
        }
    }
}

void Aya_StatQueryDistanceAndSubLevel(int kind, int value, int *distance_out, int *sublevel_out) {
    int saved_value;
    int *distance_ptr;
    int *sublevel_ptr;
    int *table;
    int index;
    int step;
    int offset;
    int *loop_cursor;
    int *cursor;
    int sublevel;
    int distance;

    saved_value = value;
    distance_ptr = distance_out;
    sublevel_ptr = sublevel_out;
    table = Aya_GetStatGrowthTable(kind);

    index = 0x40;
    step = 0x20;
    do {
        offset = index << 2;
        loop_cursor = (int *)(offset + (int)table);
        if (saved_value >= loop_cursor[0]) {
            index += step;
        } else if (saved_value < loop_cursor[-1]) {
            index -= step;
        } else {
            step = 0;
        }
        step >>= 1;
    } while (step != 0);

    index--;
    step = 0x62;
    if (index < 0x63) {
        step = index;
    }
    index = step;

    if (index < 0x62) {
        offset = index << 2;
        cursor = (int *)(offset + (int)table);
        distance = cursor[1] - saved_value;
    } else {
        distance = 0;
    }

    if (index < 0x62) {
        offset = index << 2;
        cursor = (int *)(offset + (int)table);
        sublevel = InterpolateSubLevel(saved_value, cursor);
    } else {
        sublevel = 0;
    }

    if (distance_ptr != 0) {
        *distance_ptr = distance;
    }
    if (sublevel_ptr != 0) {
        if (index < 0x62) {
            *sublevel_ptr = sublevel;
        } else {
            *sublevel_ptr = 0x30;
        }
    }
}

int Aya_StatOverflowQuery(int arg0, int value) {
    int *table;
    int step;
    int index;
    int result;

    table = Aya_GetStatGrowthTable(arg0);
    index = 0x40;
    step = 0x20;

    while (step != 0) {
        if (value >= table[index]) {
            index += step;
        } else if (value < table[index - 1]) {
            index -= step;
        } else {
            step = 0;
        }
        step >>= 1;
    }

    index--;
    result = 0x62;
    if (index < 0x63) {
        result = index;
    }

    if (result <= 0) {
        return 0;
    }
    return value - table[result - 1];
}

int g_SaveMetadataWindowIndex;

void Save_ResetMetadataWindow(void) {
    g_SaveMetadataWindowIndex = 0;
}

void Save_PostInitStub(void) {
}

int Save_GetMetadataWindowIndex(void) {
    return g_SaveMetadataWindowIndex;
}

void Save_SelectMetadataWindow(ItemDataRecord *record) {
    SaveMetadataWindow *window;

    g_SaveMetadataSourceRecord = record;
    if (record != 0) {
        window = g_CursorRenderMetadataWindows;
        if (record->kind == ITEM_KIND_ARMOR) {
            window++;
        }
    } else {
        window = g_SaveRuntimeState.windows.metadata;
        if (g_SaveMetadataWindowIndex != 0) {
            window++;
        }
    }

    g_SaveMetadataCursor = window->text;
    if (record == 0) {
        g_SaveMetadataWindowLength = g_SaveMetadataWindowIndex != 0 ? 8 : 6;
    } else {
        g_SaveMetadataWindowLength = 6;
    }
}

void Save_AppendMetadataChar(unsigned char ch) {
    unsigned char *cursor;
    int used;
    int length;
    unsigned int value;
    int has_room;
    int terminator;
    /* Never used: an unused local array is what gives this leaf function
     * its 8-byte stack frame, in retail as here. */
    unsigned char spare[4];

    length = g_SaveMetadataWindowLength;
    cursor = g_SaveMetadataCursor;
    used = 0;
    if (length > 0) {
        terminator = 0xFF;

        do {
            value = *cursor;
            if (value != terminator) {
                used += value < 0xFA;
                cursor++;
            } else {
                break;
            }
        } while (used < length);

        has_room = used < g_SaveMetadataWindowLength;
        if (has_room != 0) {
            cursor[0] = ch;
            cursor[1] = 0xFF;
            return;
        }
    }

    if (cursor[-2] >= 0xFA) {
        cursor--;
        cursor[0] = 0xFF;
    }

    cursor[-1] = ch;
}

int Save_DeleteMetadataChar(void) {
    unsigned char *cursor = g_SaveMetadataCursor;
    int used = 0;
    int removed;
    int length = g_SaveMetadataWindowLength;

    while (used < length && *cursor != 0xFF) {
        used += *cursor < 0xFA;
        cursor++;
    }

    removed = 0 < used;
    if (removed != 0) {
        cursor--;
        cursor[0] = 0xFF;
        cursor--;
        if (cursor >= g_SaveMetadataCursor && *cursor >= 0xFA) {
            cursor[0] = 0xFF;
        }
    }

    return removed;
}

void Save_LoadMetadataWindowText(void) {
    unsigned char *cursor;
    unsigned char *text;
    unsigned char *lookup;
    unsigned char *dst;
    unsigned char *out;
    unsigned char value;
    unsigned char loop_value;
    int loop_terminator;

    cursor = g_SaveMetadataCursor;
    if (cursor < g_SaveMetadataCursor + g_SaveMetadataWindowLength) {
        do {
            *cursor = 0xFF;
            cursor++;
        } while (cursor < g_SaveMetadataCursor + g_SaveMetadataWindowLength);
    }

    dst = g_SaveMetadataCursor;
    if (g_SaveMetadataSourceRecord != 0) {
        lookup = Text_LookupTable8(g_SaveMetadataSourceRecord->itemId - 1);
        out = dst;
    } else {
        lookup = Text_LookupTable4(0x1E);
        out = dst;
    }

    text = lookup;
    value = *text++;
    *out++ = value;
    if (value != 0xFF) {
        loop_terminator = 0xFF;
        do {
            loop_value = *text++;
            *out++ = loop_value;
        } while (loop_value != loop_terminator);
    }
}

unsigned char *Save_GetMetadataCursor(void) {
    return g_SaveMetadataCursor;
}

char *Save_GetActiveMetadataBuffer(void) {
    char *buffer;

    buffer = g_SaveRuntimeState.windows.metadata[0].text;

    if (g_SaveMetadataWindowIndex != 0) {
        buffer = g_SaveRuntimeState.windows.metadata[1].text;
    }

    return buffer;
}

unsigned int Save_GetMetadataRemainingChars(void) {
    unsigned int remaining = g_SaveMetadataWindowLength;
    unsigned char *cursor = g_SaveMetadataCursor;

    while (remaining != 0 && *cursor != 0xFF) {
        remaining -= *cursor < 0xFA;
        cursor++;
    }

    return remaining;
}

SavedMenuMode g_SavedMenuMode;

/* Equips the inventory item whose equipment record has this ID: weapons
 * (kinds 1..8) and armor (kind 9). Returns nonzero when nothing was equipped. */
int Inv_EquipItemById(unsigned int itemId) {
    int result = itemId;
    ItemDataRecord *record = g_InventoryRuntime.equipment;

    if (record < g_InventoryRuntime.equipment + 128) {
        while (1) {
            if (record->itemId == result) {
                break;
            }
            record++;
            if (record >= g_InventoryRuntime.equipment + 128) {
                break;
            }
        }
        if (record < g_InventoryRuntime.equipment + 128) {
            int value;
            int index;
            int count;
            s16 *item;
            s16 *end;
            g_InvItemPtr = g_InventoryRuntime.slots;
            count = Inv_GetAyaSlotLimit();
            g_InvSelectionBits = g_InvSelectionBitStorage;
            g_InvSelectionBitWords = 2;
            value = record - g_InventoryRuntime.equipment + 0x100;
            item = g_InvItemPtr;
            g_InvSlotLimit = count;
            end = item + count;
            if (item < end) {
                while (*item != value) {
                    item++;
                    if (item >= end) {
                        break;
                    }
                }
                if (item < g_InvItemPtr + g_InvSlotLimit) {
                    index = item - g_InvItemPtr;
                } else {
                    index = -1;
                }
            } else {
                index = -1;
            }
            result = index;
            if (result >= 0) {
                ItemDataRecord *entry;
                int kind;
                if (result < g_InvSlotLimit) {
                    entry = Inv_LookupItemRecordScaled(g_InvItemPtr[result]);
                } else {
                    entry = 0;
                }
                kind = 0;
                if (entry != 0) {
                    kind = entry->kind;
                }
                if (kind != 0) {
                    /* Retail tests kind < 9 first, then 9; the inner test is
                     * threaded into those two branches. */
                    if (kind < 9 || kind == 9) {
                        if (kind < 9) {
                            g_AyaEquippedWeaponSlot[0] = result;
                        } else {
                            g_AyaEquippedArmorSlot[0] = result;
                        }
                        return (unsigned int)result >> 31;
                    }
                    result = -1;
                } else {
                    result = -1;
                }
            }
        }
    }
    return (unsigned int)result >> 31;
}

void Menu_ResetInputState(void) {
    g_SavedMenuMode.value = Menu_GetActiveMode() & 0xFF;
    Battle_SetWindowBoundsByMode(0);
}

/* Open the player inventory using the same saved menu mode. */

void BattleCmd_SyncActiveAmmo(void);
void Menu_CreateInventoryTabView(void);

void Inv_OpenAyaItemList(unsigned int arg) {
    g_MenuBattleEquipMode = arg;
    g_SavedMenuMode.value = Menu_GetActiveMode() & 0xFF;
    Battle_SetWindowBoundsByMode(0);
    BattleCmd_SyncActiveAmmo();
    g_InvItemPtr = g_AyaInventoryItems;
    g_InvSlotLimit = Inv_GetAyaSlotLimit();
    g_InvSelectionBits = g_InvSelectionBitStorage;
    g_InvSelectionBitWords = 2;
    Menu_CreateInventoryTabView();
    Menu_PlayConfirmSound();
}

/* Memory-card dialog gating from the inventory frontend. */

void MemCard_SetDialogActive(int active) {
    if (active == 0) {
        g_MemCardDialogState = 0;
        MemCard_CloseAll();
        g_GameState.flags &= 0xFFFF3FFF;
    } else if (g_MemCardDialogState == 0) {
        g_MemCardDialogState = 1;
        g_GameState.flags |= 0xC000;
    }
}

int Menu_GetItemContextFlag(void);
int Save_ClassifyPlaytime(int arg0);

void Save_BuildHeader(void) {
    u16 *dst;
    int i;
    int category;
    unsigned int value;

    i = 0;
    dst = g_SaveRuntimeState.categoryCounts;
    category = 0;
    while (i < 6) {
        *dst = g_InvCategoryItemTable[category].count;
        category++;
        i++;
        dst++;
    }

    g_SaveRuntimeState.drawBlendColor = Draw_GetBlendColor();
    g_SaveRuntimeState.menuClampValue = Menu_GetItemContextFlag();
    g_SaveRuntimeState.windows.header.playTimeMinutes =
        (unsigned int)GAME_TIME_COUNTER(2) / 60U;
    g_SaveRuntimeState.windows.header.gameTimeMinutes =
        (unsigned int)GAME_TIME_COUNTER(0) / 60U;
    g_SaveRuntimeState.windows.header.drawBaseY = Draw_GetBaseY();
    value = Menu_GetActionSubmenuSelection();
    g_SaveRuntimeState.windows.header.menuLayout =
        value | (MenuWidget_GetColumnLayoutMode() << 2);
    g_SaveRuntimeState.windows.header.reserved1E = 0;
    g_SaveRuntimeState.playTimeClass = Save_ClassifyPlaytime(g_PlayTimeFrameCounter);
    g_SaveRuntimeState.currentMapNumber = Save_GetCurrentMapNumber();
}

void Menu_CopyPromptCodes(u8 *src);

void Save_RestoreHeader(void) {
    u16 *src;
    int i;
    int category;
    u8 *prompt;

    i = 0;
    src = g_SaveRuntimeState.categoryCounts;
    category = 0;
    while (i < 6) {
        g_InvCategoryItemTable[category].count = *src++;
        i++;
        category++;
    }

    Draw_BlendColor(g_SaveRuntimeState.drawBlendColor);
    Menu_ClampRange(g_SaveRuntimeState.menuClampValue);

    GAME_TIME_COUNTER(2) = (g_SaveRuntimeState.windows.header.playTimeMinutes * 15) * 4;
    GAME_TIME_COUNTER(0) = (g_SaveRuntimeState.windows.header.gameTimeMinutes * 15) * 4;
    Draw_SetBaseOffsetPosition(0, g_SaveRuntimeState.windows.header.drawBaseY - Draw_GetBaseY());
    Menu_SetActionSubmenuSelection(g_SaveRuntimeState.windows.header.menuLayout & 3);
    MenuWidget_SetColumnLayoutMode((g_SaveRuntimeState.windows.header.menuLayout >> 2) & 1);

    prompt = g_SaveRuntimeState.windows.metadata[0].text;
    if (g_SaveMetadataWindowIndex != 0) {
        prompt = g_SaveRuntimeState.windows.metadata[1].text;
    }
    Menu_CopyPromptCodes(prompt);
}

u32 g_MenuErrorSoundPending;

/* Queued by frontend commands, consumed by either frame entry point. */
void Menu_RequestErrorSound(void) {
    g_MenuErrorSoundPending = 1;
}

s32 Menu_RunFrameWithArg(s32 arg0) {
    if (Menu_SaveBgIsFadeActive() == 0) {
        g_MenuActiveListTarget = (u8 *)arg0;
        Menu_ClearCommandResult();
        Draw_SelectBuffer();
        Menu_DeferredExec();
        MenuInput_DispatchQueuedEvents();
        Menu_ProcessSwapReturnIfPending();
        MemCard_DelayedCallback();
        MenuWidget_UpdateAndDraw();
        Draw_PresentFrame(1);
        if (g_MemCardDialogState >= 2) {
            MemCard_UpdateSavePolling();
        } else if (g_MemCardDialogState > 0) {
            g_MemCardDialogState += 1;
        }
        if (g_MenuErrorSoundPending != 0) {
            g_MenuErrorSoundPending = 0;
            Inv_SetActiveList(9, 0);
        }
        if (Menu_GetCommandResult() != 0) {
            Battle_SetWindowBoundsByMode(g_SavedMenuMode.bytes.mode);
        }
        return Menu_GetCommandResult();
    }
    Menu_SaveBgAdvanceFade();
    return 0;
}

s32 Menu_RunFrame(void) {
    if (Menu_SaveBgIsFadeActive() == 0) {
        g_MenuActiveListTarget = 0;
        Menu_ClearCommandResult();
        Draw_SelectBuffer();
        Menu_DeferredExec();
        MenuInput_DispatchQueuedEvents();
        Menu_ProcessSwapReturnIfPending();
        MemCard_DelayedCallback();
        MenuWidget_UpdateAndDraw();
        Draw_PresentFrame(1);
        if (g_MemCardDialogState >= 2) {
            MemCard_UpdateSavePolling();
        } else if (g_MemCardDialogState > 0) {
            g_MemCardDialogState += 1;
        }
        if (g_MenuErrorSoundPending != 0) {
            g_MenuErrorSoundPending = 0;
            Inv_SetActiveList(9, 0);
        }
        if (Menu_GetCommandResult() != 0) {
            Battle_SetWindowBoundsByMode(g_SavedMenuMode.bytes.mode);
        }
        return Menu_GetCommandResult();
    }
    Menu_SaveBgAdvanceFade();
    return 0;
}

static inline ItemDataRecord *LookupActiveItem(int index) {
    if (index >= 0 && index < g_InvSlotLimit) return Inv_LookupItemRecordScaled(g_InvItemPtr[index]);
    return 0;
}

static inline void ClearBits(void) {
    int i;
    for (i = 0; i < g_InvSelectionBitWords; i++) g_InvSelectionBits[i] = 0;
}

/* Build the threshold-qualified set; negate its count if a tagged record exists. */
int Inv_RebuildWithBonusSlots(int minSecond, int minThird) {
    int count, i, index;
    ItemDataRecord *item;
    g_InvItemPtr = g_AyaInventoryItems;
    g_InvSlotLimit = Inv_GetAyaSlotLimit();
    g_InvSelectionBits = g_InvSelectionBitStorage;
    g_InvSelectionBitWords = 2;
    Inv_BuildFilteredPackedList(g_InvRebuildFilterMode ? 0x1FE : 0x200);
    ClearBits();
    count = 0;
    i = 0;
    while (1) {
        if (i >= g_InvPackedListCount) break;
        index = i >= 0 && i < g_InvPackedListCount ? g_InvPackedList[i] : 0;
        item = LookupActiveItem(index);
        if (item->baseStats[0] + item->bonusStats[0] >= g_InvRebuildMinFirstStat &&
            item->baseStats[1] + item->bonusStats[1] >= minSecond &&
            item->baseStats[2] + item->bonusStats[2] >= minThird &&
            item->itemId != 0x93 && item->itemId != 0x61) {
            count++;
            g_InvSelectionBits[i >> 5] |= 1u << (i & 31);
        }
        i++;
    }
    if (count) {
        ItemDataRecord *scan;
        if (g_InvRebuildFilterMode) {
            scan = g_InventoryRuntime.equipment;
            for (; scan < g_InventoryRuntime.equipment + 128 &&
                   !(scan->iconId && scan->kind < 9 && (scan->flags & 0x10));
                 scan++)
                ;
        } else {
            scan = g_InventoryRuntime.equipment;
            for (; scan < g_InventoryRuntime.equipment + 128 &&
                   !(scan->iconId && scan->kind == 9 && (scan->flags & 0x10));
                 scan++)
                ;
        }
        if (scan < g_InventoryRuntime.equipment + 128) count = -count;
    }
    g_InvPackedListCursor = count != 0;
    return count;
}

/* Historical name: mark filtered records whose tail count is below byte 1. */
int Inv_TransferToStorage(void) {
    int count, i, index;
    ItemDataRecord *item;
    g_InvItemPtr = g_AyaInventoryItems;
    g_InvSlotLimit = Inv_GetAyaSlotLimit();
    g_InvSelectionBits = g_InvSelectionBitStorage;
    g_InvSelectionBitWords = 2;
    Inv_BuildFilteredPackedList(0x3FE);
    ClearBits();
    count = 0;
    i = 0;
    while (1) {
        if (i >= g_InvPackedListCount) break;
        index = i >= 0 && i < g_InvPackedListCount ? g_InvPackedList[i] : 0;
        item = LookupActiveItem(index);
        if (item->tailCount < item->tailCapacity) {
            count++;
            g_InvSelectionBits[i >> 5] |= 1u << (i & 31);
        }
        i++;
    }
    return count;
}

static inline void SelectAya(void) {
    g_InvItemPtr = g_AyaSaveState.inventory_items;
    g_InvSlotLimit = Inv_GetAyaSlotLimit();
    g_InvSelectionBits = g_InvSelectionBitStorage;
    g_InvSelectionBitWords = 2;
}

/* Carry a tagged record forward, removing its first storage-list reference. */
static inline int TakeTagged(int armor) {
    ItemDataRecord *p;
    int id = 0;
    for (p = g_InventoryRuntime.equipment; p < g_InventoryRuntime.equipment + 128; p++) {
        if ((armor ? p->kind == 9 : p->kind != 9) && (p->flags & 16)) break;
    }
    if (p < g_InventoryRuntime.equipment + 128) {
        s16 *slot;
        id = (p - g_InventoryRuntime.equipment) + 256;
        for (slot = g_InventoryRuntime.specialStorage;
             slot < g_InventoryRuntime.specialStorage + 82; slot++)
            if (*slot == id) break;
        if (slot < g_InventoryRuntime.specialStorage + 82) *slot = 0;
    }
    return id;
}

/* Remove the selected record, preserving armor-capacity bookkeeping. JP
 * releases the reserved armor slots before clearing the record. */
static inline void RemoveItem(int index) {
    int required, id;
    if (g_InvItemPtr == g_AyaInventoryItems && g_InventoryRuntime.tracked[2] == index) {
        Inv_GetActiveSlotCount(&required);
        Inv_CheckFreeSlotCapacity(required);
        g_InventoryRuntime.tracked[2] = -1;
        Inv_SetActiveList(3, 0);
    }
    id = g_InvItemPtr[index];
    g_InvItemPtr[index] = 0;
    if (id >= 256) g_InventoryRuntime.equipment[id - 256].iconId = 0;
}

static inline int FindKind(int kind, int excluded) {
    /* excluded is -1 when no slot is skipped; the address then lies before
     * the list and never matches. */
    s16 *skip = g_InvItemPtr + excluded;
    s16 *p = g_InvItemPtr;
    while (p < g_InvItemPtr + g_InvSlotLimit) {
        if (p != skip && Inv_LookupItemRecordScaled(*p)->kind == kind) break;
        p++;
    }
    if (p < g_InvItemPtr + g_InvSlotLimit) return p - g_InvItemPtr;
    return -1;
}

/* The caller supplies a fourth script argument; these commands ignore it. */
static inline int FindItem(int id) {
    s16 *p = g_InvItemPtr;
    while (p < g_InvItemPtr + g_InvSlotLimit) {
        if (*p == id) break;
        p++;
    }
    if (p < g_InvItemPtr + g_InvSlotLimit) return p - g_InvItemPtr;
    return -1;
}

static inline void SetCapacity(int value) {
    if (value > 50) value = 50;
    g_AyaSaveState.inventory_slot_count = value;
    if (g_InvItemPtr == g_AyaInventoryItems) g_InvSlotLimit = Inv_GetAyaSlotLimit();
}

static inline void CopyName(u8 *dest, u8 *source) {
    while ((*dest++ = *source++) != 255) {}
}

static inline void InitName(u8 *buffer, int length, ItemDataRecord *item) {
    u8 *p, *source, *dest;
    g_SaveMetadataWindowIndex = 0;
    g_SaveMetadataSourceRecord = item;
    g_SaveMetadataCursor = buffer;
    g_SaveMetadataWindowLength = length;
    for (p = g_SaveMetadataCursor;
         p < g_SaveMetadataCursor + g_SaveMetadataWindowLength; p++)
        *p = 255;
    dest = g_SaveMetadataCursor;
    if (g_SaveMetadataSourceRecord)
        source = Text_LookupTable8(g_SaveMetadataSourceRecord->itemId - 1);
    else
        source = Text_LookupTable4(30);
    CopyName(dest, source);
}

void Battle_SyncEquipSlots(void);

void Inv_InitNewGameInventory(void) {
    int i, capacity;
    AyaLevelStats *stats;
    u16 *growth = g_AyaStatAgility;
    for (i = 0; i < 7; i++) *growth++ = *(u16 *)Aya_GetStatGrowthTable(i);
    stats = Aya_LookupLevelStats(0);
    g_AyaSaveState.max_hp = g_AyaSaveState.current_hp = stats->hp;
    g_AyaSaveState.inventory_slot_count = stats->inventoryCapacity;
    g_AyaSaveState.parasite_spell_flags = 1;
    g_InvSpecialItemRecords[0].ammo = g_InvSpecialItemRecords[1].ammo = g_InvSpecialItemRecords[2].ammo = 0;
    SelectAya();
    capacity = stats->inventoryCapacity;
    if (capacity > 50) capacity = 50;
    g_AyaSaveState.inventory_slot_count = capacity;
    if (g_InvItemPtr == g_AyaSaveState.inventory_items)
        g_InvSlotLimit = Inv_GetAyaSlotLimit();
    SelectAya();
    for (i = 49; i >= 0; i--) g_InvItemPtr[i] = 0;
    Inv_AddItem(0x44);
    Inv_AddItem(0x96);
    Inv_AddItem(0x3F);
    Inv_AddItem(1);
    Inv_AddItem(6);
    i = TakeTagged(0);
    if (i) Inv_AddItem(i);
    i = TakeTagged(1);
    if (i) Inv_AddItem(i);
    g_InventoryRuntime.tracked[2] = 1;
    g_InventoryRuntime.tracked[0] = 0;
    g_AyaSaveState.menu_clamp_value = 61;
    Menu_ClampRange(61);
    g_AyaSaveState.total_exp = 0;
    g_AyaSaveState.level = 0;
    Menu_SaveBgInitFade();
}

void Inv_MergeStorageToSlot(void) {
    int i;
    g_InvItemPtr = g_AyaInventoryItems;
    g_InvSlotLimit = Inv_GetAyaSlotLimit();
    g_InvSelectionBits = g_InvSelectionBitStorage;
    g_InvSelectionBitWords = 2;
    for (i = 0; i < g_InvSlotLimit; i++) {
        unsigned short id = g_InvItemPtr[i];
        if (id - 256U < 128U &&
            g_InventoryRuntime.equipment[(short)id - 256].itemId == 0x61) break;
    }
    if (i < g_InvSlotLimit) RemoveItem(i);
    Inv_AddItem(0x93);
    g_InventoryRuntime.tracked[0] = FindKind(7, -1);
    g_MenuBattleEquipMode = 0;
    Inv_SetActiveList(2, 0);
}

int Inv_DispatchCommand(int command, int value, int other, int *unused) {
    switch (command) {
    case INV_CMD_COUNT_OCCUPIED: {
        s16 *p;
        int count = 0;
        SelectAya();
        for (p = g_InvItemPtr; p < g_InvItemPtr + g_InvSlotLimit; p++)
            count += (*p != 0);
        return count;
    }
    case INV_CMD_GET_CAPACITY: {
        int bonus = Inv_GetBonusSlotCount();
        if (g_InvBaseCapacity + bonus < 51) {
            bonus = Inv_GetBonusSlotCount();
            return g_InvBaseCapacity + bonus;
        }
        return 50;
    }
    case INV_CMD_COUNT_ITEM: {
        int count = 0;
        s16 *p;
        if (value >= g_InvCategoryBaseItemId && value < g_InvCategoryBaseItemId + 3)
            count = g_InvSpecialItemRecords[value - g_InvCategoryBaseItemId].ammo;
        else for (p = g_InvItemPtr; p < g_InvItemPtr + g_InvSlotLimit; p++) {
            unsigned short id = *p;
            if (id - 256U < 128U)
                count += (g_InventoryRuntime.equipment[(short)id - 256].itemId == value);
            else
                count += ((short)id == value);
        }
        return count;
    }
    case INV_CMD_SET_REBUILD_FILTER:
        g_InvRebuildFilterMode = value;
        g_InvRebuildMinFirstStat = other;
        return 0;
    case INV_CMD_REBUILD_SLOTS:
        return Inv_RebuildWithBonusSlots(value, other);
    case INV_CMD_SET_CAPACITY:
        SetCapacity(value);
        return 0;
    case INV_CMD_GET_CURRENT_HP:
        return g_AyaSaveState.current_hp;
    case INV_CMD_GET_MAX_HP:
        return g_AyaSaveState.max_hp;
    case INV_CMD_SET_CURRENT_HP:
        BattleCmd_SetCurrentHP(value);
        return 0;
    case INV_CMD_GET_REMAINING_AMMO:
        return BattleCmd_GetRemainingAmmo(0);
    case INV_CMD_GET_AMMO_VALUE: {
        int result;
        BattleCmd_GetRemainingAmmo(&result);
        return result;
    }
    case INV_CMD_SET_CURRENT_MP:
        BattleCmd_SetCurrentMP(value);
        return 0;
    case INV_CMD_REMOVE_ITEM: {
        int index;
        SelectAya();
        index = FindItem(value);
        if (index < 0) return index;
        Inv_RemoveActiveListItem(index);
        return 0;
    }
    case INV_CMD_OPEN_STORAGE:
        Inv_TransferToStorage();
        MenuInput_InitQueue();
        Menu_StepInventoryRoot(0, -3, -1);
        Menu_CreateContextHelpPanel();
        return 0;
    case INV_CMD_COMPUTE_GAMMA:
        Menu_ComputeGammaLut(value, other);
        return 0;
    case INV_CMD_INIT_MEMCARD:
        MemCard_InitSlotState();
        return 0;
    case INV_CMD_START_SAVE_FADE:
        Menu_SaveBgStartFadeOut();
        return 0;
    case INV_CMD_START_NEW_GAME:
        Menu_SetMemCardConfirmPending();
        if (g_AyaSaveState.new_game_count + 1 < 100) g_AyaSaveState.new_game_count++;
        else g_AyaSaveState.new_game_count = 99;
        Inv_InitNewGameInventory();
        return 0;
    case INV_CMD_INIT_BONUS_POINTS:
        Menu_InitBonusPointAllocState(value);
        return 0;
    case INV_CMD_CLEAR_STORAGE_ITEM: {
        s16 *p;
        int missing;
        for (p = g_InventoryRuntime.storage; p < g_InventoryRuntime.storage + 100; p++)
            if (*p == value) break;
        missing = !(p < g_InventoryRuntime.storage + 100);
        if (!missing) *p = 0;
        return missing;
    }
    case INV_CMD_MERGE_STORAGE:
        Inv_MergeStorageToSlot();
        /* Fall through: this command returns 0. */
    }
    return 0;
}

void Save_InitMetadataState(void) {
    bzero(&g_SaveRuntimeState, sizeof(g_SaveRuntimeState));
    /* Retail initializes the same window twice, including both lookups. JP
     * fills the six-character save name and leaves the prompt codes alone. */
    InitName(g_SaveRuntimeState.windows.header.saveName, 6, 0);
    InitName(g_SaveRuntimeState.windows.header.saveName, 6, 0);
    Inv_InitNewGameInventory();
    g_InventoryRuntime.specialStorage[0] = 515;
    g_AyaSaveState.blend_color = 0x404040;
    Draw_BlendColor(0x404040);
    g_GameTimers[0].current = 0;
    g_GameTimers[1].current = 0;
    g_GameTimers[2].current = 0;
    g_GameTimers[3].current = 0;
    Draw_SetBaseOffsetPosition(0, 8 - Draw_GetBaseY());
    MenuWidget_SetColumnLayoutMode(0);
    Menu_SetActionSubmenuSelection(1);
    g_CursorRenderDataBlock[0] = 255;
    g_CursorRenderDataBlock[0x10] = 255;
}

int Save_GetCurrentMapNumber(void) {
    char roomName[8];

    Text_EncodeBase32(roomName, g_SceneDispatchToken);
    return Text_ParseMapNumber(roomName);
}

void Menu_InitScrollDirection(void) {
    g_MenuScrollStep = g_MenuScrollDirectionValueUnwritten < 0x12C ? 4 : -4;
}

void Inv_InitMaxLevelInventory(s32 arg0) {
    u16 *nextStat;
    u16 *stat;
    s32 allocation;
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
    g_AyaInventorySlotCount[0] = 0x32;
    if (g_InvItemPtr == items) {
        g_InvSlotLimit = Inv_GetAyaSlotLimit();
    }
    g_AyaSaveLevel[0] = 0x62;
    expTable = Aya_GetLevelExpTable();
    g_AyaSaveTotalExp[0] = expTable[g_AyaSaveState.level];
    levelStats = Aya_LookupLevelStats(0x62);
    unlockedSpells = 0xFFFFF;
    save = (AyaSaveState *)((u8 *)items - PE1_OFFSETOF(AyaSaveState, inventory_items));
    nextStat = save->stats.levels;
    maxHp = levelStats->hp;
    category = 0;
    g_AyaParasiteSpellFlags[0] = unlockedSpells;
    g_AyaHpCurrent[0] = maxHp;
    g_AyaHpMax[0] = maxHp;
    do {
        stat = nextStat;
        nextStat = stat + 1;
        if (mode == 0) {
            allocation = 0x3E8;
        } else {
            allocation = category - 1U < 2U;
            if (allocation != 0) {
                allocation = 0;
            } else {
                allocation = 0x3E8;
            }
        }
        *stat = allocation;
        category += 1;
    } while (category < 7);
    Battle_SyncEquipSlots();
}
