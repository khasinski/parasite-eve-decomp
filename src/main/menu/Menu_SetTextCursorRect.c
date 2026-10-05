/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: --use-comm-section -G8 */

#include "pe1/textbox_open.h"

void Menu_SetTextCursorRect(int x, int y, int w, int h) {
    D_8009CE98.x = x;
    D_8009CE98.y = y;
    D_8009CE98.width = w;
    D_8009CE98.height = h;
}
