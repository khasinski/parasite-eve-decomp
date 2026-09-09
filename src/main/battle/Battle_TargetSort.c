typedef struct BattleTargetDistance {
    int distance;
    int fields08_0C[2];
} BattleTargetDistance;

typedef struct BattleTargetRecord {
    int field_0;
    int field_4;
    int field_8;
} BattleTargetRecord;

extern BattleTargetDistance D_8009E004[];

void Battle_SwapRecords(BattleTargetRecord *records, int from, int to);

void Battle_SortTargets(BattleTargetRecord *records, signed char first, signed char last)
{
    signed char pivot;
    signed char scan;

    if (first < last) {
        Battle_SwapRecords(records, first, (signed char)((first + last) / 2));
        pivot = first;
        for (scan = pivot; scan <= last; scan++) {
            if (D_8009E004[(signed char)scan].distance < D_8009E004[first].distance) {
                pivot++;
                Battle_SwapRecords(records, (signed char)pivot, (signed char)scan);
            }
        }
        Battle_SwapRecords(records, first, pivot);
        Battle_SortTargets(records, first, (signed char)(pivot - 1));
        Battle_SortTargets(records, (signed char)(pivot + 1), last);
    }
}

void Battle_SwapRecords(BattleTargetRecord *records, int from_arg, int to_arg) {
    BattleTargetRecord tmp;
    int from_index;
    int to_index;

    from_index = (signed char)from_arg;
    to_index = (signed char)to_arg;

    tmp = records[from_index];
    records[from_index] = records[to_index];
    records[to_index] = tmp;
}
