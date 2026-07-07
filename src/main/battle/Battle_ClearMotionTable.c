
typedef unsigned char u8;

extern int g_BattleEntitySlotInUse[];

void Battle_ClearMotionTable(void) {
    u8 i;
    int offset;

    i = 0;
    do {
        offset = (((i * 7) << 3) - i) << 2;
        *(int *)((char *)g_BattleEntitySlotInUse + offset) = 0;
        i++;
    } while (i < 7);
}
