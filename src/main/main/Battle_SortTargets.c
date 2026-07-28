/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
typedef signed char s8;
typedef unsigned char u8;
typedef int s32;

extern volatile s32 D_8009E004[];

#define TARGET_DISTANCE(index) D_8009E004[(s8)(index) * 3]

void Battle_SwapRecords(void *records, int a, int b);

void Battle_SortTargets(void *records, int lo, int hi) {
    s8 left;
    s8 right;
    s8 pivot;
    s8 scan;
    s8 store;

    left = lo;
    right = hi;
    if (left < right) {
        Battle_SwapRecords(records, left, (s8)((left + right) / 2));

        store = lo;
        scan = store;
        while (scan <= right) {
            if (TARGET_DISTANCE(scan) < TARGET_DISTANCE(left)) {
                store++;
                Battle_SwapRecords(records, store, scan);
            }
            scan++;
        }

        pivot = store;
        Battle_SwapRecords(records, left, pivot);
        Battle_SortTargets(records, left, (s8)(pivot - 1));
        Battle_SortTargets(records, (s8)(pivot + 1), right);
    }
}
