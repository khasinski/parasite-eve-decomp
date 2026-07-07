/* MASPSX_FLAGS: --expand-div */

#include "include_asm.h"

int *Stat_GetGrowthTable(int arg0);

int Stat_BinarySearch(int value, int *table) {
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

INCLUDE_ASM("asm/USA/main/nonmatchings/item/stat", Stat_QueryLevelAndSubLevel);

void Stat_QueryDistanceAndSubLevel(int kind, int value, int *distance_out, int *sublevel_out) {
    register int saved_value asm("$16");
    register int *distance_ptr asm("$18");
    register int *sublevel_ptr asm("$17");
    register int *table asm("$6");
    int index;
    register int step asm("$3");
    register int offset asm("$2");
    register int *loop_cursor asm("$4");
    register int *cursor asm("$2");
    int sublevel;
    int distance;

    saved_value = value;
    distance_ptr = distance_out;
    sublevel_ptr = sublevel_out;
    table = Stat_GetGrowthTable(kind);

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
        {
            register int current_reg asm("$4");
            register int next_reg asm("$6");
            register int delta_reg asm("$3");
            register int numerator_reg asm("$2");

            next_reg = cursor[1];
            current_reg = cursor[0];
            if (next_reg != current_reg) {
                delta_reg = saved_value - current_reg;
                numerator_reg = ((delta_reg << 1) + delta_reg) * 16 + delta_reg;
                delta_reg = next_reg - current_reg;
                sublevel = numerator_reg / delta_reg;
            } else {
                sublevel = 0;
            }
        }
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

int Stat_OverflowQuery(int arg0, int value) {
    int *table;
    int step;
    int index;
    int result;

    table = Stat_GetGrowthTable(arg0);
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
