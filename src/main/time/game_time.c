extern unsigned int g_GameTimeTable[][3];

int GameTime_GetZero(void)
{
    return 0;
}

unsigned int GameTime_GetCounterSeconds(int arg0) {
    return g_GameTimeTable[arg0][0] / 60U;
}

void GameTime_SetCounterSeconds(int arg0, int arg1) {
    g_GameTimeTable[arg0][0] = arg1 * 60;
}
