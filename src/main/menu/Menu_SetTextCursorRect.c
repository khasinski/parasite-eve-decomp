/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: --use-comm-section -G8 */

extern short D_8009CE98;
extern short D_8009CE9A;
extern short D_8009CE9C;
extern short D_8009CE9E;

void Menu_SetTextCursorRect(int x, int y, int w, int h) {
    D_8009CE98 = x;
    D_8009CE9A = y;
    D_8009CE9C = w;
    D_8009CE9E = h;
}
