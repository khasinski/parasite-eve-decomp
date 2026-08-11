#include "pe1/battle.h"

void Battle_SortTargets(char *records, int first, int last) {
    int pivot;
    int scan;

    first = (signed char)first;
    last = (signed char)last;
    if (first < last) {
        Battle_SwapRecords(records, first, (first + last) / 2);
        pivot = first;
        scan = pivot;
        while (scan <= last) {
            if (g_BattleTargetList[(signed char)scan].dist <
                g_BattleTargetList[first].dist) {
                pivot++;
                Battle_SwapRecords(records, (signed char)pivot,
                                   (signed char)scan);
            }
            scan++;
        }
        Battle_SwapRecords(records, first, (signed char)pivot);
        Battle_SortTargets(records, first, (signed char)(pivot - 1));
        Battle_SortTargets(records, (signed char)(pivot + 1), last);
    }
}
