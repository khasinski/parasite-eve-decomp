#define PE1_PM_LEGACY_RAW_VIEWS
#include "pe1/pm.h"
#undef PE1_PM_LEGACY_RAW_VIEWS
extern int g_GameState;

int Pm_FreeSlot(int arg0) {
    int offset;
    PmSlotHeader *entry;
    unsigned int i;

    if ((unsigned int)arg0 >= 0x16) {
        return -1;
    }

    if ((unsigned int)arg0 >= 0xB) {
        int idx;
        int offset_hi;
        idx = arg0 - 0xB;
        offset_hi = idx << 4;
        offset_hi += idx;
        offset_hi <<= 2;
        offset_hi -= idx;
        entry = (PmSlotHeader *)(g_PmSlotTable2Raw + (offset_hi << 2));
    } else {
        offset = (((((arg0 * 4) + arg0) << 5) + arg0) << 2) - arg0;
        offset <<= 2;
        entry = (PmSlotHeader *)(g_PmSlotTableRaw + offset);
    }

    if (entry->command == 0x72) {
        for (i = 0x6C; i < 0x73; i++) {
            g_PmAuxiliaryPointerTable.entries[i - 0x6C] = 0;
        }
        {
            int *state = &g_GameState;
            *state &= 0xFFFEFFFF;
        }
    }

    entry->state = 0;
    entry->command = -1;
    entry->field02 = -1;
    entry->field03 = -1;
    entry->ticks = 0;
    entry->owner = 0;

    return 0;
}
#include "pe1/cdrom.h"
extern PmCommand **D_800942E0;
extern PmPrimarySlot *D_800942E4;
extern PmSecondarySlot *D_800942E8;
extern u32 D_800B0CD8[], D_800B0DD8;
extern u16 D_80093162[];
extern void *D_80011618;
extern u8 D_801F1BD8[], D_801F1C58[], D_801F1D00[], D_801F1D8C[];
extern u8 D_801F1E18[], D_801F1EA4[], D_801F1EF0[];
void EnterCriticalSection(void);
void FlushCache(void);
void ExitCriticalSection(void);
void Asset_LoadTimTextures(int);
void func_800CE49C(PmSlotHeader *, int);

/* Matching debt: the retry jump preserves the two CD retry loops;
 * secondary-slot addressing uses unsigned address arithmetic. */
static inline int FindSlot(u32 command)
{
    int slot = -1;
    int i;
    if (command >= 192)
        return -1;
    if (command - 70 < 15) {
        PmSecondarySlot *entry = D_800942E8;
        for (i = 0; i < 11; ++i, ++entry)
            if (!entry->header.state) {
                slot = i + 11;
                break;
            }
    } else {
        PmPrimarySlot *entry = D_800942E4;
        for (i = 0; i < 11; ++i, ++entry)
            if (!entry->header.state) {
                slot = i;
                break;
            }
    }
    return slot;
}

int Scene_LoadRoomAssets(u32 command, void *owner)
{
    u32 original;
    int subcommand = 0;
    int slot, ready;
    PmSlotHeader *entry;
    if (command >= 192)
        return -7;
    if (command - 108 < 7 && !(D_800B0CD8[0] & 0x10000)) {
retry:
        while (CdRom_ReadSectorsFromLba(D_800B0DD8 + D_80093162[0],
                D_80011618, D_80093162[1] - D_80093162[0]) == -1) {}
        for (;;) {
            ready = CdRom_PollReady();
            if (ready == 0)
                break;
            if (ready == -1)
                goto retry;
        }
        EnterCriticalSection();
        FlushCache();
        ExitCriticalSection();
        g_PmAuxiliaryPointerTable.entries[0] = D_801F1BD8;
        g_PmAuxiliaryPointerTable.entries[1] = D_801F1C58;
        g_PmAuxiliaryPointerTable.entries[2] = D_801F1D00;
        g_PmAuxiliaryPointerTable.entries[3] = D_801F1D8C;
        g_PmAuxiliaryPointerTable.entries[4] = D_801F1E18;
        g_PmAuxiliaryPointerTable.entries[5] = D_801F1EA4;
        g_PmAuxiliaryPointerTable.entries[6] = D_801F1EF0;
        D_800B0CD8[0] |= 0x10000;
    }
    Asset_LoadTimTextures(0);
    original = command;
    if (command >= 85) {
        subcommand = command - 85;
        command = 85;
    }
    if (!D_800942E0[command])
        return -8;
    if (!D_800942E0[command]->initialize)
        return -1;
    slot = FindSlot(command);
    if (slot == -1)
        return -3;
    if ((u32)slot >= 22)
        return -1;
    if (slot >= 11)
        entry = (PmSlotHeader *)((slot - 11) * sizeof(PmSecondarySlot) + (u32)D_800942E8);
    else
        entry = &D_800942E4[slot].header;
    entry->state = 1;
    entry->command = original;
    entry->field02 = 0;
    entry->field03 = 0;
    entry->ticks = 0;
    entry->owner = owner;
    if (command == 85)
        func_800CE49C(entry, subcommand);
    D_800942E0[command]->initialize(entry);
    return slot;
}
