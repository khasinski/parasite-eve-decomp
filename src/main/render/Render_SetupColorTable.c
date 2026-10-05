#include "common.h"
#include "pe1/textbox_open.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

/* Opens the first free textbox for message `index`. A styled box takes the
 * current text rectangle; `values` (ended by -1, at most five) fill the
 * message's number slots as decimal digits.
 * Matching debt: five register pins and three empty barriers. The reciprocal
 * operand preserves constant-hoist order; the digit memory operand keeps its
 * base across the inner loop; the final count operand preserves the counter.
 * The page-index register is reused for slot initialization. No CPU ASM. */
void Render_SetupColorTable(int inputIndex, int inputStyle, short *inputValues)
{
    unsigned char style = inputStyle;
    register short *values asm("$12") = inputValues;
    unsigned char i;
    register short index asm("$9") = inputIndex;

    for (i = 0; i < 4; i++) {
        unsigned char slot;

        if (g_TextboxEntries[i].state != 0)
            continue;

        g_TextboxEntries[i].state = 1;
        g_TextboxEntries[i].background = 0;
        g_TextboxEntries[i].page_id = index;
        D_8009CEA0 = 0;
        {
            int terminator = -1;
            D_8009CEA4 = terminator;
        }
        g_TextboxEntries[i].style = style;
        g_TextboxEntries[i].control.flags &= ~0x100000;
        {
            int reciprocal = 0x66666667;
            asm("" : : "r"(reciprocal), "r"(index));
        }
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

        index = 0;
        for (slot = index; slot < 5; slot++) {
            register unsigned short inputNumber asm("$7") = *values++;
            short value = inputNumber;
            register unsigned short quotient asm("$5");
            register unsigned char count asm("$8");

            if (value == -1)
                return;
            count = 0;
            g_TextboxEntries[i].numbers[slot].digits[count] = value - (quotient = value / 10) * 10;
            asm volatile("" : : "m"(g_TextboxEntries[i].numbers[slot].digits[count]) : "$6");
            value = quotient;
            while (value != 0) {
                unsigned short quotient;
                quotient = value / 10;
                count++;
                g_TextboxEntries[i].numbers[slot].digits[count] = value - quotient * 10;
                value = quotient;
            }
            g_TextboxEntries[i].numbers[slot].count = count + 1;
            asm("" : : "r"(count));
        }
        return;
    }
}
