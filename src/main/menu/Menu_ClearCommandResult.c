/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

extern int D_8009D010;

void Menu_ClearCommandResult(void) {
    D_8009D010 = 0;
}
