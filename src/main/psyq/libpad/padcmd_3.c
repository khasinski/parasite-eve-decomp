/* ASSEMBLER: GNU */
/* PSY-Q LIBPAD PADCMD, part 3 of 7: CardObj_EmitReadTransferCommand, LIBPAD_PADCMD_text_3A0. */
#include "pe1/card_obj.h"

void CardObj_EmitCommand4C(CardObj *arg0, unsigned char arg1);
void CardObj_EmitCommand46(CardObj *arg0, unsigned char arg1);
void CardObj_EmitCommand47(CardObj *arg0, unsigned char arg1);
void CardObj_EmitCommand4B(CardObj *arg0);

void CardObj_EmitReadTransferCommand(CardObj *arg0) {
    int state = arg0->field_46;

    switch (state) {
    case 2:
        CardObj_EmitCommand4C(arg0, arg0->field_47);
        break;
    case 3:
        CardObj_EmitCommand46(arg0, arg0->field_47);
        break;
    case 4:
        if (arg0->field_48 == 0) {
            CardObj_EmitCommand47(arg0, arg0->field_47);
        } else {
            CardObj_EmitCommand4B(arg0);
        }
        break;
    }
}

/* Psy-Q LIBPAD/PADCMD.OBJ private text_3A0.
 * Provenance: configs/USA/psyq_provenance.json (LIBPAD PADCMD). */
extern u8 *D_800A5AD0;
int LIBPAD_PADCMD_text_3A0(CardObj *inPort) {
    register CardObj *port = inPort;
    register int result asm("$2");
    switch (port->field_46) {
    case 2:
        ((u16 *)port->field_00)[port->field_47] =
            port->response_3c[5] + (port->response_3c[4] << 8);
        port->field_47++;
        if (port->field_47 >= port->field_e3) {
            port->field_47 = 0;
            goto complete;
        }
        result = 0;
        break;
    case 3: {
        PadCapabilityRecord *record =
            (PadCapabilityRecord *)port->field_04 + port->field_47;
        record->bytes[0] = port->response_3c[4];
        record->bytes[1] = port->response_3c[5] & 127;
        record->bytes[2] = port->response_3c[6];
        record->bytes[3] = port->response_3c[7];
        {
            register int high = port->response_3c[5];
            record->high_bit = high >> 7;
        }
        port->field_47++;
        if (port->field_47 >= port->field_e9) {
            port->field_47 = 0;
            port->field_48 = 0;
            goto complete;
        }
        result = 0;
        break;
    }
    case 4: {
        PadDataRecord *record = (PadDataRecord *)port->field_08 + port->field_47;
        register u8 *source, *base;
        register int bytes asm("$4");
        unsigned offset;
        if (port->field_48 == 0) {
            {
                register int length = port->response_3c[4];
                bytes = 3;
                port->field_48 = length;
            }
            record->length = port->field_48;
            {
                register u8 *response = port->response_3c;
                register int index = port->field_47;
                source = response + 5;
                if (index == 0) {
                    base = port->field_08;
                    offset = port->field_ea * 8;
                } else {
                    base = record[-1].data;
                    offset = (record[-1].length + 3) & 0x1fc;
                }
            }
            base = base + offset;
            record->data = base;
            D_800A5AD0 = base;
        } else {
            register u8 *response = port->response_3c;
            bytes = 6;
            source = response + 2;
        }
        bytes--;
        if (bytes != -1) {
            register u8 **destination = &D_800A5AD0;
            do {
                register int remaining = port->field_48;
                bytes--;
                if (!remaining)
                    goto exhausted;
                {
                    register u8 *dst = *destination;
                    register int value = *source++;
                    asm("" : "=r"(source) : "0"(source));
                    *dst = value;
                    *destination = dst + 1;
                }
                port->field_48--;
            } while (bytes != -1);
        }

        if (port->field_48 == 0)
            goto exhausted;
    zero:
        asm("" ::: "memory");
        result = 0;
        break;
    exhausted:
        {
            port->field_47++;
            if (port->field_47 >= port->field_ea) {
                port->field_49 = 6;
                port->field_46 = 254;
                result = 0;
                break;
            }
            port->field_48 = 0;
        }
        goto zero;
    }
    default:
        result = 1;
        break;
    }
    return result;
complete:
    result = 1;
    return result;
}
