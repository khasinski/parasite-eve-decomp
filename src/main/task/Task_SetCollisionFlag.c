/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

extern int D_8009CED4;

void Task_SetCollisionFlag(int value) {
    D_8009CED4 = value != 0;
}
