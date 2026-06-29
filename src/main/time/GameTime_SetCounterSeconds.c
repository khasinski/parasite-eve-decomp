extern unsigned int g_GameTimeTable[][3];

void GameTime_SetCounterSeconds(int arg0, int arg1) {
    g_GameTimeTable[arg0][0] = arg1 * 60;
}
