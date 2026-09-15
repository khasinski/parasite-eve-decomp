/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
#include "pe1/save.h"
#include "pe1/textbox.h"

void Menu_SaveOverlayDraw(void)
{
    short color;
    int x, y;
    /* Matching debt: preserve the reloaded notification state in a0. */
    register int state asm("$4");
    switch (D_8009D1CE) {
    case 1:
        color = -1;
        Tbl_ResetAll();
        x = Save_GetMetadataWindowIndex() ? 20 : 97;
        y = D_8009CE80 < 2 ? 15 : 195;
        Menu_SetTextCursorRect(x, y, 0, 0);
        Render_SetupColorTable(0, 2, &color);
        state = D_8009D1CE;
        g_TextboxEntries[0].state = 2;
        D_8009CE88 = 75;
        /* Publish the active textbox and timer before reading its message. */
        asm volatile("" : : : "memory");
        g_TextboxEntries[0].message = D_8009D1F8;
        state++;
        D_8009D1CE = state;
        g_TextboxEntries[0].flags |= 0x2000000;
        break;
    case 2:
        if (!D_8009CE88) {
            Tbl_ResetAll();
            D_8009D1CE = 0;
        }
        break;
    }
    --D_8009CE88;
    Draw_SetCursor(0, D_8009CE80 < 2 ? 11 : 191);
    Draw_AllocColorGradient(320, 20, 0, 0);
}
