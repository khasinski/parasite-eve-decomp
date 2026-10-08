#include "pe1/game_timers.h"

int GameTime_GetZero(void)
{
    return 0;
}

unsigned int GameTime_GetCounterSeconds(int arg0) {
    return (unsigned int)GAME_TIME_COUNTER(arg0) / 60U;
}

void GameTime_SetCounterSeconds(int arg0, int arg1) {
    GAME_TIME_COUNTER(arg0) = arg1 * 60;
}
