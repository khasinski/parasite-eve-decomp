extern unsigned int g_GameTimeTable[][3];

unsigned int GameTime_GetCounterSeconds(int arg0) {
    return g_GameTimeTable[arg0][0] / 60U;
}
