#include "common.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

extern s8 D_8009CED0;
extern int D_8009CED4;

void Task_EnableMovement(void) {
    D_8009CED0 = 1;
}

void Task_DisableMovement(void) {
    D_8009CED0 = 0;
}

void Task_SetCollisionFlag(int value) {
    D_8009CED4 = value != 0;
}
