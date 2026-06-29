/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

extern int g_FieldMoveLock[];

int Task_GetPlayerInputFlag(int **arg0) {
    if (g_FieldMoveLock[0] & 1) {
        *arg0[0] = 0;
    } else {
        *arg0[0] = 1;
    }
    return 1;
}
