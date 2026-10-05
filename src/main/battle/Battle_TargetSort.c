#include "pe1/battle.h"

extern BattleTargetWords D_8009E004[];

void Battle_SwapRecords(BattleTargetWords *records, int from, int to);

void Battle_SortTargets(BattleTargetWords *records, signed char first, signed char last)
{
    signed char pivot;
    signed char scan;

    if (first < last) {
        Battle_SwapRecords(records, first, (signed char)((first + last) / 2));
        pivot = first;
        for (scan = pivot; scan <= last; scan++) {
            if (D_8009E004[(signed char)scan].word_00 < D_8009E004[first].word_00) {
                pivot++;
                Battle_SwapRecords(records, (signed char)pivot, (signed char)scan);
            }
        }
        Battle_SwapRecords(records, first, pivot);
        Battle_SortTargets(records, first, (signed char)(pivot - 1));
        Battle_SortTargets(records, (signed char)(pivot + 1), last);
    }
}

void Battle_SwapRecords(BattleTargetWords *records, int from_arg, int to_arg) {
    BattleTargetWords tmp;
    int from_index;
    int to_index;

    from_index = (signed char)from_arg;
    to_index = (signed char)to_arg;

    tmp = records[from_index];
    records[from_index] = records[to_index];
    records[to_index] = tmp;
}
