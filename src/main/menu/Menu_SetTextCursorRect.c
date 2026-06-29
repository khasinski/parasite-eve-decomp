/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

short D_8009CE98;
short D_8009CE9A;
short D_8009CE9C;
short D_8009CE9E;

void Menu_SetTextCursorRect(int arg0, int arg1, int arg2, int arg3) {
    D_8009CE98 = arg0;
    D_8009CE9A = arg1;
    D_8009CE9C = arg2;
    D_8009CE9E = arg3;
}
