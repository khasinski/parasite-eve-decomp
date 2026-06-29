/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

signed char g_PlayerMovementEnabled;

void Task_DisableMovement(void) {
    g_PlayerMovementEnabled = 0;
}
