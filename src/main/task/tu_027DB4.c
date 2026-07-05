/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: --use-comm-section -G8 */

signed char g_PlayerMovementEnabled;

int g_TaskCollisionFlag;

void Task_EnableMovement(void) {
    g_PlayerMovementEnabled = 1;
}

void Task_DisableMovement(void) {
    g_PlayerMovementEnabled = 0;
}

void Task_SetCollisionFlag(int arg0) {
    g_TaskCollisionFlag = arg0 != 0;
}
