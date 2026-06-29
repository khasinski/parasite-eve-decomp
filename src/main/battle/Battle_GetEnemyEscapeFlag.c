extern unsigned int *g_PlayerEntity;

int Battle_GetEnemyEscapeFlag(void)
{
    unsigned int *ptr;

    if (g_PlayerEntity != 0) {
        ptr = (int *)g_PlayerEntity[0];
        if (ptr != 0) {
            return (ptr[0x13] >> 9) & 1;
        }
    }

    return 0;
}
