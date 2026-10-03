/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: --use-comm-section -G8 */
#include "pe1/textbox.h"

void Render_SetupColorTable(int page, int style, short *args)
{
    unsigned char i;
    unsigned char arg;
    unsigned char count;
    short value;
    unsigned short quotient;

    for (i = 0; i < 4; i++) {
        if (g_TextboxEntries[i].state == 0) {
            g_TextboxEntries[i].state = 1;
            g_TextboxEntries[i].typed = 0;
            g_TextboxEntries[i].page_id = page;
            D_8009CEA0 = 0;
            D_8009CEA4 = -1;
            g_TextboxEntries[i].style = style;
            g_TextboxEntries[i].flags &= ~0x100000;
            g_TextboxEntries[i].flags &= ~0x200000;
            if ((unsigned char)style != 0) {
                g_TextboxEntries[i].x = D_8009CE98;
                g_TextboxEntries[i].y = D_8009CE9A;
                g_TextboxEntries[i].color0 = D_8009CE9C;
                g_TextboxEntries[i].color1 = D_8009CE9E;
                if (g_TextboxEntries[i].style == 3) {
                    g_TextboxEntries[i].flags |= 0x100000;
                }
            } else if (D_8009CED0 != 0) {
                g_TextboxEntries[i].typed = 1;
            }
            for (arg = 0; arg < 5; arg++) {
            value = *args++;
            if (value == -1) {
                return;
            }
            count = 0;
            quotient = value / 10;
            g_TextboxEntries[i].args[arg][0] = value - quotient * 10;
            value = quotient;
            while (value != 0) {
                quotient = value / 10;
                count++;
                g_TextboxEntries[i].args[arg][count] = value - quotient * 10;
                value = quotient;
            }
            g_TextboxEntries[i].args[arg][5] = count + 1;
            }
            return;
        }
    }
}
