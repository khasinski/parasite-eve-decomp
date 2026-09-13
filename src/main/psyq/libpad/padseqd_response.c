/* ASSEMBLER: GNU */
#include "pe1/psyq_pad_main.h"
int CardObj_IsTransferActive(CardObj *);
int CardObj_AdvanceReadLayout(CardObj *);
void LIBPAD_PADSEQD_text_108(CardObj *port) {
    int old = port->field_e8;
    int i;
    int mode = port->response_3c[0] >> 4;
    port->field_e8 = mode;
    if (mode == 15)
        port->field_e8 = old;
    else {
        port->output_30[0] = 0;
        port->output_30[1] = port->response_3c[0];
        for (i = 2; i < port->response_index; i++)
            port->output_30[i] = port->response_3c[i];
    }
    if (port->response_3c[1] == 0 && (port->field_46 != 1 || port->fn_14 != 0) &&
        port->pad_50[0] == 0)
        goto reset;
    if (!CardObj_IsTransferActive(port) && !port->saved_command && !port->field_4a &&
        port->field_e8 != old) {
    reset:
        D_8009B728(port);
    }
    port->field_4a = 0;
    if ((u8)(port->field_46 - 2) < 252 && port->response_3c[0] != 0xf3)
        D_8009B728(port);
    {
        register int state = port->field_46;
        register int one;
        register int value asm("$2");
        if (state != 0 && state != 255 && !port->command)
            return;
        one = 1;
        if (state == one)
            goto initial;
        if (state < 2) {
            value = 254;
            asm("" : : "r"(value));
            if (state == 0)
                goto idle;
            goto advance;
        }
        value = 254;
        if (state == value)
            goto terminal;
        if (state == 255)
            return;
        goto advance;
    idle:
        if (!port->field_e8)
            return;
        value = port->field_46;
        port->field_49 = one;
        goto increment;
    initial:
        value = port->field_46;
        port->field_47 = 0;
    increment:
        value++;
        port->field_46 = value;
        return;
    terminal:
        value = 255;
        port->field_46 = value;
        return;
    advance:
        if (port->fn_18)
            value = port->fn_18(port);
        else
            value = CardObj_AdvanceReadLayout(port);
        port->field_46 += value;
    }
}
