extern void **g_PlayerEntity;
extern short g_AyaHpCurrent;

void BattleCmd_SetCurrentHP(int arg0) {
    void *current;

    if (g_PlayerEntity != 0) {
        current = g_PlayerEntity[0];
        if (current != 0) {
            *(short *)((char *)current + 0xC) = arg0;
        }
    }

    g_AyaHpCurrent = arg0;
}
