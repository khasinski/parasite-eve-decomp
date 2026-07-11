/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

typedef signed char s8;

extern s8 D_8009CED0;

void Task_EnableMovement(void) {
    D_8009CED0 = 1;
}
