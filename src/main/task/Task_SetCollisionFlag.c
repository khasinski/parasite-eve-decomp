/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

int g_TaskCollisionFlag;

void Task_SetCollisionFlag(int arg0) {
    g_TaskCollisionFlag = arg0 != 0;
}
