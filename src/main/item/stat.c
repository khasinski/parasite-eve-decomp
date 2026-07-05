#include "include_asm.h"

int Stat_GetGrowthTable(int arg0);

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

INCLUDE_ASM("asm/USA/main/nonmatchings/item/stat", Stat_QueryDistanceAndSubLevel);

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
