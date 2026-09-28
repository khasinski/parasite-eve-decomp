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



int Pm_SendCmd(int arg0, int arg1, int arg2, int *arg3, int *arg4, int *arg5) {
    int offset;
    char *entry;
    int cmd;
    PmCommand *handler;
    PmCommand **table;
    int table_offset;
    PmSendCallback callback;

    if ((unsigned int)arg0 >= 0x16) {
        return -0xA;
    }

    if ((unsigned int)arg0 >= 0xB) {
        int idx;
        int offset_hi;
        idx = arg0 - 0xB;
        offset_hi = idx << 4;
        offset_hi += idx;
        offset_hi <<= 2;
        offset_hi -= idx;
        entry = (char *)g_PmSlotTable2 + (offset_hi << 2);
    } else {
        offset = (((((arg0 * 4) + arg0) << 5) + arg0) << 2) - arg0;
        offset <<= 2;
        entry = (char *)g_PmSlotTable + offset;
    }

    cmd = *(u8 *)(entry + 1);
    if ((unsigned int)cmd >= 0xC0) {
        return -0xB;
    }
    if ((unsigned int)cmd >= 0x55) {
        cmd = 0x55;
    }

    table = g_PmCmdHandlerTable;
    table_offset = cmd << 2;
    handler = *(PmCommand **)(table_offset + (int)table);
    if (handler == 0) {
        return -0xC;
    }
    callback = handler->send;
    if (callback == 0) {
        return -1;
    }

    if ((arg1 == 1) && (arg2 == 0)) {
        *arg3 = *(u8 *)(entry + 2);
        *arg4 = *(u8 *)(entry + 3);
        *arg5 = *(int *)(entry + 4);
    }

    {
        register PmCommand **reload_table asm("$3");
        int reload_offset;
        reload_table = g_PmCmdHandlerTable;
        /* Match debt: preserve the table-load scheduling and operand order. */
        asm volatile("" : "=r"(cmd) : "0"(cmd) : "memory");
        reload_offset = cmd << 2;
        asm volatile("" : "=r"(reload_table), "=r"(reload_offset) : "0"(reload_table), "1"(reload_offset));
        handler = *(PmCommand **)((u32)reload_offset + (u32)reload_table);
        /* The table can change through arg3..arg5, so reload before calling. */
        return handler->send((PmSlotHeader *)entry, arg1, arg2, arg3, arg4, arg5);
    }
}

int Pm_SetGetState(int arg0, int arg1, int arg2) {
    int offset;
    int cmd;
    PmCommand *handler;

    if ((unsigned int)arg0 >= 0x16) {
        return -0xD;
    }

    if ((unsigned int)arg0 >= 0xB) {
        int idx;
        int offset_hi;
        idx = arg0 - 0xB;
        offset_hi = idx << 4;
        offset_hi += idx;
        offset_hi <<= 2;
        offset_hi -= idx;
        arg0 = (int)((char *)g_PmSlotTable2 + (offset_hi << 2));
    } else {
        offset = (((((arg0 * 4) + arg0) << 5) + arg0) << 2) - arg0;
        offset <<= 2;
        arg0 = (int)((char *)g_PmSlotTable + offset);
    }

    cmd = *(u8 *)(arg0 + 1);
    if ((unsigned int)cmd >= 0x55) {
        cmd = 0x55;
    }

    handler = g_PmCmdHandlerTable[cmd];
    if (handler == 0) {
        return -0xF;
    }

    if (arg1 == 0) {
        if ((unsigned int)arg2 < 6) {
            *(u8 *)arg0 = arg2;
        }
    } else {
        *(int *)arg2 = *(u8 *)arg0;
    }

    return *(u8 *)arg0;
}

int Pm_Start(int arg0) {
    int offset;
    register int cmd asm("$5");
    PmCommand *handler;
    int (*callback)(void);
    PmCommand **table;
    register int table_offset asm("$2");

    if ((unsigned int)arg0 >= 0x16) {
        return -0x10;
    }

    if ((unsigned int)arg0 >= 0xB) {
        int idx;
        int offset_hi;
        idx = arg0 - 0xB;
        offset_hi = idx << 4;
        offset_hi += idx;
        offset_hi <<= 2;
        offset_hi -= idx;
        arg0 = (int)((char *)g_PmSlotTable2 + (offset_hi << 2));
    } else {
        offset = (((((arg0 * 4) + arg0) << 5) + arg0) << 2) - arg0;
        offset <<= 2;
        arg0 = (int)((char *)g_PmSlotTable + offset);
    }

    if ((unsigned int)(*(u8 *)arg0 - 1) >= 2U) {
        return 0;
    }

    cmd = *(u8 *)(arg0 + 1);
    if ((unsigned int)cmd >= 0xC0) {
        return -0x11;
    }
    if ((unsigned int)cmd >= 0x55) {
        cmd = 0x55;
    }

    table = g_PmCmdHandlerTable;
    table_offset = cmd << 2;
    handler = *(PmCommand **)(table_offset + (int)table);
    if (handler == 0) {
        return -0x12;
    }
    callback = handler->start;
    if (callback != 0) {
        return callback();
    }

    return -1;
}


#define PE1_GAME_STATE_LEGACY_RAW_VIEW
#include "pe1/game_state.h"
#undef PE1_GAME_STATE_LEGACY_RAW_VIEW

int Pm_Exec(int arg0) {
    register int offset asm("$2");
    char *entry;
    int state;
    int cmd;
    PmCommand *handler;
    int (*callback)(PmSlotHeader *);
    PmCommand **table;
    int table_offset;
    unsigned int i;

    if ((unsigned int)arg0 >= 0x16) {
        return -0x13;
    }

    if ((unsigned int)arg0 >= 0xB) {
        int idx;
        int offset_hi;
        idx = arg0 - 0xB;
        offset_hi = idx << 4;
        offset_hi += idx;
        offset_hi <<= 2;
        offset_hi -= idx;
        entry = g_PmSlotTable2Raw + (offset_hi << 2);
    } else {
        offset = (((((arg0 * 4) + arg0) << 5) + arg0) << 2) - arg0;
        offset <<= 2;
        entry = g_PmSlotTableRaw + offset;
    }

    state = *(u8 *)entry;
    if (((unsigned int)(state - 1) >= 2U) && ((unsigned int)(state - 4) >= 2U)) {
        return 0;
    }

    {
        int state2;
        asm volatile("" ::: "memory");
        state2 = *(u8 *)entry;
        state = state2;
    }
    if (state == 4) {
        *(u8 *)entry = 5;
        return 0;
    }
    if (state == 5) {
        if ((unsigned int)arg0 < 0x16) {
            char *cleanup;

            if ((unsigned int)arg0 >= 0xB) {
                int idx;
                int offset_hi;
                idx = arg0 - 0xB;
                offset_hi = idx << 4;
                offset_hi += idx;
                offset_hi <<= 2;
                offset_hi -= idx;
                cleanup = g_PmSlotTable2Raw + (offset_hi << 2);
            } else {
                offset = (((((arg0 * 4) + arg0) << 5) + arg0) << 2) - arg0;
                offset <<= 2;
                cleanup = g_PmSlotTableRaw + offset;
            }
            if (*(u8 *)(cleanup + 1) == 0x72) {
                for (i = 0x6C; i < 0x73; i++) {
                    g_PmAuxiliaryPointerTable.entries[i - 0x6C] = 0;
                }
                g_GameState &= 0xFFFEFFFF;
            }
            *(u8 *)cleanup = 0;
            *(u8 *)(cleanup + 1) = -1;
            *(u8 *)(cleanup + 2) = -1;
            *(u8 *)(cleanup + 3) = -1;
            *(int *)(cleanup + 4) = 0;
            *(int *)(cleanup + 8) = 0;
        }
        return 0;
    }

    if (state == 1) {
        *(u8 *)entry = 2;
    }

    cmd = *(u8 *)(entry + 1);
    if ((unsigned int)cmd >= 0xC0) {
        return -0x14;
    }
    if ((unsigned int)cmd >= 0x55) {
        cmd = 0x55;
    }

    table = g_PmCmdHandlerTable;
    table_offset = cmd << 2;
    handler = *(PmCommand **)(table_offset + (int)table);
    if (handler == 0) {
        return -0x15;
    }
    callback = handler->execute;
    if (callback != 0) {
        int ret = callback((PmSlotHeader *)entry);
        *(int *)(entry + 4) = *(int *)(entry + 4) + 1;
        return ret;
    }

    return -1;
}

int Pm_Stop(int arg0, int arg1, int arg2) {
    register int state asm("$3");
    PmCommand *handler;
    int (*callback)(void);
    int result;
    register int offset asm("$2");
    int orig;
    unsigned int i;

    orig = arg0;
    if ((unsigned int)orig >= 0x16) {
        return -0x16;
    }

    if ((unsigned int)orig >= 0xB) {
        int idx;
        int offset_hi;
        idx = orig - 0xB;
        offset_hi = idx << 4;
        offset_hi += idx;
        offset_hi <<= 2;
        offset_hi -= idx;
        arg0 = (int)(g_PmSlotTable2Raw + (offset_hi << 2));
    } else {
        offset = (((((orig * 4) + orig) << 5) + orig) << 2) - orig;
        offset <<= 2;
        arg0 = (int)(g_PmSlotTableRaw + offset);
    }
    state = *(u8 *)arg0;
    if ((state == 0) || (state == 6)) {
        return 0;
    }
    if (arg2 == 0) {
        if (*(int *)(arg0 + 8) != arg1) {
            return 0;
        }
    }

    arg1 = *(u8 *)(arg0 + 1);
    if ((unsigned int)arg1 >= 0xC0) {
        return -0x17;
    }
    if ((unsigned int)arg1 >= 0x55) {
        arg1 = 0x55;
    }

    handler = g_PmCmdHandlerTable[arg1];
    if (handler == 0) {
        return -0x18;
    }
    callback = handler->stop;
    if (callback == 0) {
        return -1;
    }

    result = callback();

    if ((unsigned int)orig < 0x16) {
        if ((unsigned int)orig >= 0xB) {
            int idx;
            int offset_hi;
            idx = orig - 0xB;
            offset_hi = idx << 4;
            offset_hi += idx;
            offset_hi <<= 2;
            offset_hi -= idx;
            arg1 = (int)(g_PmSlotTable2Raw + (offset_hi << 2));
        } else {
            offset = (((((orig * 4) + orig) << 5) + orig) << 2) - orig;
            offset <<= 2;
            arg1 = (int)(g_PmSlotTableRaw + offset);
        }
        if (*(u8 *)(arg1 + 1) == 0x72) {
            for (i = 0x6C; i < 0x73; i++) {
                g_PmAuxiliaryPointerTable.entries[i - 0x6C] = 0;
            }
            g_GameState &= 0xFFFEFFFF;
        }
        *(u8 *)arg1 = 0;
        *(u8 *)(arg1 + 1) = -1;
        *(u8 *)(arg1 + 2) = -1;
        *(u8 *)(arg1 + 3) = -1;
        *(int *)(arg1 + 4) = 0;
        *(int *)(arg1 + 8) = 0;
    }

    return result;
}


static inline void clearSlot(int slot)
{
    PmSlotHeader *entry;
    unsigned int i;
    if ((unsigned int)slot < 22) {
        if ((unsigned int)slot >= 11) {
            /* Signed byte offset preserves the retail loop strength reduction. */
            entry = (PmSlotHeader *)((u8 *)g_PmSlotTable2Typed +
                (slot - 11) * (int)sizeof(PmSecondarySlot));
        } else {
            entry = &g_PmSlotTableTyped[slot].header;
        }
        if (entry->command == 0x72) {
            for (i = 0x6C; i < 0x73; ++i)
                g_PmSlotBufferTyped[i] = 0;
            g_GameStateTyped.flags &= ~0x10000;
        }
        entry->state = 0;
        entry->command = 0xFF;
        entry->field02 = 0xFF;
        entry->field03 = 0xFF;
        entry->ticks = 0;
        entry->owner = 0;
    }
}
int Scene_FreeEntityTable(void *owner)
{
    int result;
    int i;
    if (!owner)
        return -25;
    result = 0;
    for (i = 0; i < 11; ++i) {
        PmSlotHeader *entry = &g_PmSlotTableTyped[i].header;
        if (entry->owner == owner) {
            result = Pm_Stop(i, (int)owner, 1);
            if (result)
                return result;
            clearSlot(i);
        }
    }
    for (i = 0; i < 11; ++i) {
        PmSlotHeader *entry = &g_PmSlotTable2Typed[i].header;
        if (entry->owner == owner) {
            result = Pm_Stop(i + 11, (int)owner, 1);
            if (result)
                return result;
            clearSlot(i + 11);
        }
    }
    return result;
}


extern s32 g_PlayerEntity[];
#define g_PlayerEntity (g_PlayerEntity[0])
/* Separate symbol views retain the retail read/modify/write instruction order. */
extern s32 D_800B0CD8_r[] __asm__("g_GameState");
extern s32 D_800B0CD8_w[] __asm__("g_GameState");

s32 Pm_StopAll(void) {
    u8 *clear_base;
    u8 *base_v0;
    u32 *var_v1;
    s32 var_a2;
    s32 var_s1;
    s32 var_s2;
    s32 var_v0;
    u32 var_a0;
    u32 var_s0;
    u8 temp_v1;
    PmSlotHeader *var_a1;

    var_a2 = 0;
    var_s0 = 0;
    clear_base = (u8 *)g_PmSlotBufferTyped;
    var_s2 = -0xB84;
    var_s1 = 0;
loop_1:
    base_v0 = (u8 *)g_PmSlotTableRaw;
    temp_v1 = ((PmSlotHeader *)(base_v0 + var_s1))->command;
    if ((temp_v1 < 8U) || ((u32)(temp_v1 - 0x55) < 0x1EU)) {
        var_v0 = Pm_Stop(var_s0, g_PlayerEntity, 1);
        var_a2 = var_v0;
        asm volatile("" : "=r"(var_a2) : "0"(var_a2));
        if (var_a2 == 0) {
            if (var_s0 < 0x16U) {
                if (var_s0 >= 0xBU) {
                    var_a1 = (PmSlotHeader *)(g_PmSlotTable2Raw + var_s2);
                } else {
                    base_v0 = (u8 *)g_PmSlotTableRaw;
                    var_a1 = (PmSlotHeader *)(base_v0 + var_s1);
                }
                if (var_a1->command == 0x72) {
                    var_a0 = 0x6C;
                    var_v1 = (s32 *)(clear_base + 0x1B0);
                    do {
                        *var_v1 = 0;
                        var_a0 += 1;
                        var_v1 += 1;
                    } while (var_a0 < 0x73U);
                    D_800B0CD8_w[0] = D_800B0CD8_r[0] & 0xFFFEFFFF;
                }
                var_a1->state = 0;
                var_a1->command = 0xFFU;
                var_a1->field02 = 0xFFU;
                var_a1->field03 = 0xFFU;
                var_a1->ticks = 0;
                var_a1->owner = 0;
            }
            goto block_13;
        }
    } else {
block_13:
        var_s2 += 0x10C;
        var_s0 += 1;
        var_s1 += 0xA0C;
        if ((s32)var_s0 >= 0xB) {
            var_v0 = var_a2;
        } else {
            goto loop_1;
        }
    }
    return var_v0;
}


/* Both passes share the PM slot ABI but walk their banks in opposite order. */
s32 Pm_StopUpperHalf(void) {
    u32 loop_index;
    u8 *clear_base;
    s32 table1_offset;
    s32 table2_offset;
    u32 slot;
    s32 result;
    PmSlotHeader *entry;
    u8 *base;
    register u32 fill asm("$2");
    u32 marker;
    u32 clear_index;
    u32 *clear_ptr;
    register s32 mask asm("$3");
    register s32 game_state asm("$2");

    loop_index = 0;
    clear_base = (u8 *)g_PmSlotBufferTyped;
    table2_offset = 0x6E84;
    table1_offset = 0;
    slot = 0xB;
    do {
        {
            s32 arg0 = slot;
            s32 arg1 = 0;
            s32 arg2 = 1;
            result = Pm_Stop(arg0, arg1, arg2);
        }
        if (result != 0) {
            return result;
        }

        if (slot < 0x16U) {
            if (slot >= 0xBU) {
                base = (u8 *)g_PmSlotTable2Typed;
                entry = (PmSlotHeader *)(base + table1_offset);
            } else {
                base = (u8 *)g_PmSlotTableTyped;
                entry = (PmSlotHeader *)(base + table2_offset);
            }

            marker = entry->command;
            fill = 0x72;
            if (marker != fill) {
                fill = 0xFF;
                goto clear_entry;
            }
            fill = 0xFF;
            clear_index = 0x6C;
            clear_ptr = (s32 *)(clear_base + 0x1B0);
            do {
                *clear_ptr = 0;
                clear_index++;
                clear_ptr++;
            } while (clear_index < 0x73U);
            mask = 0xFFFEFFFF;
            game_state = D_800B0CD8_r[0];
            game_state &= mask;
            D_800B0CD8_w[0] = game_state;
            fill = 0xFF;

clear_entry:
            entry->state = 0;
            entry->command = fill;
            entry->field02 = fill;
            entry->field03 = fill;
            entry->ticks = 0;
            entry->owner = 0;
        }

        table2_offset += 0xA0C;
        table1_offset += 0x10C;
        loop_index++;
        slot++;
    } while ((s32)loop_index < 0xB);

    return result;
}

s32 Pm_StopLowerHalf(void) {
    u32 slot;
    s32 table1_offset;
    s32 table2_offset;
    u8 *clear_base;
    s32 result;
    PmSlotHeader *entry;
    u8 *base;
    register u32 fill asm("$2");
    u32 marker;
    u32 clear_index;
    u32 *clear_ptr;
    register s32 mask asm("$3");
    register s32 game_state asm("$2");

    slot = 0;
    clear_base = (u8 *)g_PmSlotBufferTyped;
    table1_offset = 0;
    table2_offset = -0xB84;
    do {
        {
            s32 arg0 = slot;
            s32 arg1 = 0;
            s32 arg2 = 1;
            result = Pm_Stop(arg0, arg1, arg2);
        }
        if (result != 0) {
            return result;
        }

        if (slot < 0x16U) {
            if (slot >= 0xBU) {
                base = (u8 *)g_PmSlotTable2Typed;
                entry = (PmSlotHeader *)(base + table2_offset);
            } else {
                base = (u8 *)g_PmSlotTableTyped;
                entry = (PmSlotHeader *)(base + table1_offset);
            }

            marker = entry->command;
            fill = 0x72;
            if (marker != fill) {
                fill = 0xFF;
                goto clear_entry;
            }
            fill = 0xFF;
            clear_index = 0x6C;
            clear_ptr = (s32 *)(clear_base + 0x1B0);
            do {
                *clear_ptr = 0;
                clear_index++;
                clear_ptr++;
            } while (clear_index < 0x73U);
            mask = 0xFFFEFFFF;
            game_state = D_800B0CD8_r[0];
            game_state &= mask;
            D_800B0CD8_w[0] = game_state;
            fill = 0xFF;

clear_entry:
            entry->state = 0;
            entry->command = fill;
            entry->field02 = fill;
            entry->field03 = fill;
            entry->ticks = 0;
            entry->owner = 0;
        }

        table1_offset += 0xA0C;
        slot++;
        table2_offset += 0x10C;
    } while ((s32)slot < 0xB);

    return result;
}

void Pm_StopAllBoth(void) {
    Pm_StopLowerHalf();
    Pm_StopUpperHalf();
}
