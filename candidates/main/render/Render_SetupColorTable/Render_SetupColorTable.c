#include "common.h"
#include "pe1/textbox_open.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

/* Opens the first free textbox for message `index`. A styled box takes the
 * current text rectangle; `values` (ended by -1, at most five) fill the
 * message's number slots as decimal digits. */
void Render_SetupColorTable(short index, unsigned char style, short *values)
{
    unsigned char i;

    for (i = 0; i < 4; i++) {
        TextboxEntry *entry;
        unsigned char slot;

        if (g_TextboxEntries[i].state != 0)
            continue;

        g_TextboxEntries[i].state = 1;
        g_TextboxEntries[i].background = 0;
        g_TextboxEntries[i].page_id = index;
        D_8009CEA0 = 0;
        D_8009CEA4 = -1;
        g_TextboxEntries[i].style = style;
        g_TextboxEntries[i].control.flags &= ~0x100000;
        g_TextboxEntries[i].control.flags &= ~0x200000;
        if (style) {
            g_TextboxEntries[i].x = D_8009CE98.x;
            g_TextboxEntries[i].y = D_8009CE98.y;
            g_TextboxEntries[i].width = D_8009CE98.width;
            g_TextboxEntries[i].height = D_8009CE98.height;
            if (g_TextboxEntries[i].style == 3)
                g_TextboxEntries[i].control.flags |= 0x100000;
        } else if (D_8009CED0) {
            g_TextboxEntries[i].background = 1;
        }

        for (slot = 0; slot < 5; slot++) {
            short value = *values++;
            unsigned short quotient;
            unsigned char count;
            unsigned char *digits;

            if (value == -1)
                return;
            count = 0;
            digits = g_TextboxEntries[i].numbers[slot].digits;
            quotient = value / 10;
            digits[0] = value - quotient * 10;
            value = quotient;
            while (value != 0) {
                quotient = value / 10;
                count++;
                digits[count] = value - quotient * 10;
                value = quotient;
            }
            g_TextboxEntries[i].numbers[slot].count = count + 1;
        }
        return;
    }
}
