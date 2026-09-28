#include "common.h"
#define PE1_GAME_STATE_LEGACY_RAW_VIEW
#define PE1_PM_LEGACY_RAW_VIEWS
#include "pe1/game_state.h"
#include "pe1/pm.h"
#undef PE1_GAME_STATE_LEGACY_RAW_VIEW
#undef PE1_PM_LEGACY_RAW_VIEWS
extern char *g_PmSlotTable;
extern char *g_PmSlotTable2;
extern int **g_PmCmdHandlerTable;
extern int g_GameState;
extern int D_800E10A0[];

int Pm_Exec(int arg0) {
    register int offset asm("$2");
    char *entry;
    int state;
    int cmd;
    int **handler;
    int (*callback)(char *);
    int **table;
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
        entry = g_PmSlotTable2 + (offset_hi << 2);
    } else {
        offset = (((((arg0 * 4) + arg0) << 5) + arg0) << 2) - arg0;
        offset <<= 2;
        entry = g_PmSlotTable + offset;
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
                cleanup = g_PmSlotTable2 + (offset_hi << 2);
            } else {
                offset = (((((arg0 * 4) + arg0) << 5) + arg0) << 2) - arg0;
                offset <<= 2;
                cleanup = g_PmSlotTable + offset;
            }
            if (*(u8 *)(cleanup + 1) == 0x72) {
                for (i = 0x6C; i < 0x73; i++) {
                    D_800E10A0[i - 0x6C] = 0;
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
    handler = *(int ***)(table_offset + (int)table);
    if (handler == 0) {
        return -0x15;
    }
    callback = (int (*)(char *))handler[4];
    if (callback != 0) {
        int ret = callback(entry);
        *(int *)(entry + 4) = *(int *)(entry + 4) + 1;
        return ret;
    }

    return -1;
}

int Pm_Stop(int arg0, int arg1, int arg2) {
    register int state asm("$3");
    int **handler;
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
        arg0 = (int)(g_PmSlotTable2 + (offset_hi << 2));
    } else {
        offset = (((((orig * 4) + orig) << 5) + orig) << 2) - orig;
        offset <<= 2;
        arg0 = (int)(g_PmSlotTable + offset);
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

    handler = (int **)g_PmCmdHandlerTable[arg1];
    if (handler == 0) {
        return -0x18;
    }
    callback = (int (*)(void))handler[5];
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
            arg1 = (int)(g_PmSlotTable2 + (offset_hi << 2));
        } else {
            offset = (((((orig * 4) + orig) << 5) + orig) << 2) - orig;
            offset <<= 2;
            arg1 = (int)(g_PmSlotTable + offset);
        }
        if (*(u8 *)(arg1 + 1) == 0x72) {
            for (i = 0x6C; i < 0x73; i++) {
                D_800E10A0[i - 0x6C] = 0;
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
    s32 *var_v1;
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
    base_v0 = (u8 *)g_PmSlotTable;
    temp_v1 = ((PmSlotHeader *)(base_v0 + var_s1))->command;
    if ((temp_v1 < 8U) || ((u32)(temp_v1 - 0x55) < 0x1EU)) {
        var_v0 = Pm_Stop(var_s0, g_PlayerEntity, 1);
        var_a2 = var_v0;
        asm volatile("" : "=r"(var_a2) : "0"(var_a2));
        if (var_a2 == 0) {
            if (var_s0 < 0x16U) {
                if (var_s0 >= 0xBU) {
                    var_a1 = (PmSlotHeader *)(g_PmSlotTable2 + var_s2);
                } else {
                    base_v0 = (u8 *)g_PmSlotTable;
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
