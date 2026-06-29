extern unsigned char g_BattleEnemyEscapeFlag;

void Task_SignalExit(void) {
    g_BattleEnemyEscapeFlag = 1;
}
