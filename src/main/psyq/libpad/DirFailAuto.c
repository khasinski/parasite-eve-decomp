/* ASSEMBLER: GNU */
/* Psy-Q LIBPAD/PADSEQD.OBJ _dirFailAuto.
 * See proposals/PsyqPadDsFour for byte-match evidence and constraint debt. */
#include "pe1/psyq_pad_main.h"
void MemCard_OutputHandler(CardObj *port) {
    port->field_4c++;
    switch (port->field_46) {
    case 0:
        break;
    case 1:
        if (port->field_4a < 2)
            port->field_4a++;
        else {
            port->field_49 = 2;
            port->field_46 = 255;
        }
        return;
    default:
        if (port->field_4a < 4) {
            port->field_4a++;
            return;
        }
        if (port->field_49)
            D_8009B728(port);
    }
    if (*port->response_3c != 0xf3) {
        port->output_30[0] = 255;
        port->output_30[1] = 0;
        port->field_e8 = 0;
    }
}
