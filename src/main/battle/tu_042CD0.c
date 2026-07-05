extern unsigned int *g_PlayerEntity;

extern unsigned short g_AyaStatAgility;

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

int Battle_GetAgilityBonus(void)
{
    return g_AyaStatAgility;
}
