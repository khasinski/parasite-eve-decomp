extern void **g_PlayerEntity;

void BattleCmd_SetCurrentMP(int arg0) {
    void *current;

    if (g_PlayerEntity != 0) {
        current = g_PlayerEntity[0];
        if (current != 0) {
            *(int *)((char *)current + 8) = arg0 << 16;
        }
    }
}
