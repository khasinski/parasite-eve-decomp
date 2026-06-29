extern unsigned int g_GameStateFlags;

int Battle_GetStateFlag1(void)
{
    return (g_GameStateFlags >> 1) & 1;
}
