#include "common.h"
s32 Pm_Stop(s32 slot, s32 entity, s32 mode);

extern struct { char _[16]; } D_800942E4_o __asm__("g_PmSlotTable");
extern struct { char _[16]; } D_800942E8_o __asm__("g_PmSlotTable2");
extern struct { char _[16]; } D_800E0EF0_o __asm__("g_PmSlotBuffer");
extern s32 D_800B0CD8_r[] __asm__("g_GameState");
extern s32 D_800B0CD8_w[] __asm__("g_GameState");

#define g_PmSlotTable (*(u8 **)&D_800942E4_o)
#define g_PmSlotTable2 (*(u8 **)&D_800942E8_o)
#define g_PmSlotBuffer (*(u8 *)&D_800E0EF0_o)

s32 Pm_StopLowerHalf(void) {
    u32 slot;
    s32 table1_offset;
    s32 table2_offset;
    u8 *clear_base;
    s32 result;
    u8 *entry;
    u8 *base;
    register u32 fill asm("$2");
    u32 marker;
    u32 clear_index;
    s32 *clear_ptr;
    register s32 mask asm("$3");
    register s32 game_state asm("$2");

    slot = 0;
    clear_base = &g_PmSlotBuffer;
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
                base = g_PmSlotTable2;
                entry = base + table2_offset;
            } else {
                base = g_PmSlotTable;
                entry = base + table1_offset;
            }

            marker = entry[1];
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
            entry[0] = 0;
            entry[1] = fill;
            entry[2] = fill;
            entry[3] = fill;
            *(s32 *)(entry + 4) = 0;
            *(s32 *)(entry + 8) = 0;
        }

        table1_offset += 0xA0C;
        slot++;
        table2_offset += 0x10C;
    } while ((s32)slot < 0xB);

    return result;
}
