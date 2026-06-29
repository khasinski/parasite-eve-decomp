/* CC1_FLAGS: -G0 -g3 */
/* MASPSX_FLAGS: -G0 --use-comm-section */

typedef unsigned char u8;

extern int g_BattleEntitySlotInUse[];
extern char g_BattleEntitySlotArray[];

void Battle_SlotFree(void **arg0) {
    u8 i;
    char *base;
    int offset;
    char *slot;

    i = 0;
    base = g_BattleEntitySlotArray;
    do {
        offset = (((i * 7) << 3) - i) << 2;
        slot = (char *)(offset + (int)base);
        if (slot == *arg0) {
            *(int *)((char *)g_BattleEntitySlotInUse + offset) = 0;
        }
        i++;
    } while (i < 7);
    *arg0 = 0;
}
