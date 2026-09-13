/* ASSEMBLER: GNU */
#include "pe1/psyq_pad_main.h"
extern void (*D_8009B730)(CardObj *);
extern int (*D_8009B72C)(CardObj *, int);
extern void (*D_8009B744)(CardObj *), (*D_8009B748)(CardObj *);
extern int D_8009B770, D_8009B764, D_8009B79C, D_8009B77C[];
extern volatile u8 *D_8009B7A0;
int MemCard_WriteByte(CardObj *, int);
int MemCard_WaitStatusBit2(void);
int LIBPAD_PADIF_text_26C(CardObj *inPort) {
    register CardObj *port = inPort;
    register int mode;
    register int index;
    register int offset;
    register CardObj *other asm("$19");
    register int three;
    register int result;
    register CardObj *arg;
    D_8009B730(inPort);
    mode = 0;
    if (D_8009B770) {
        register int id = port->response_3c[0];
        {
            register int eight = 8;
            if (id >> 4 == eight) {
                register int command = port->command;
                mode = command == 0;
            }
        }
    }
    if (mode) {
        index = -1;
        offset = -240;
    first:
        if (--D_8009B79C <= 0)
            goto setup;
        if (index >= 0)
            D_8009B730((CardObj *)(port->field_0c + offset));
        result = MemCard_WriteByte(port, (u8)D_8009B72C(port, 1));
        if (result < 0)
            goto done;
        Timer_StartTimeout(60);
        if (!MemCard_WaitReadyForTransfer())
            goto timeout;
        index++;
        offset += 240;
        if (index < 4)
            goto first;
    }
setup:
    {
        register int original = D_8009B764;
        register int opposite;
        opposite = !original;
        if (D_8009B79C >= 2) {
            register int *count asm("$16");
            register int *table asm("$3") = D_8009B77C;
            register int scaled asm("$2");
            scaled = opposite * 4;
            count = (int *)((u32)scaled + (u32)table);
            {
                register int product = opposite * 15;
                offset = product * 16;
            }
            three = 3;
            do {
                int remaining = *count;
                if (remaining < 0)
                    break;
                if (remaining > 0) {
                    register CardObj *base asm("$3") =
                        (CardObj *)((u32)offset + (u32)D_8009B758);
                    register int scaled asm("$2") = remaining * sizeof(CardObj);
                    register u8 *child asm("$3") = base->field_0c + scaled;
                    other = (CardObj *)(child - sizeof(CardObj));
                    D_8009B744(other);
                }
                {
                    register int value = *count;
                    if (value == three)
                        goto paired;
                    if (value < 4) {
                        arg = port;
                        if (value >= 2)
                            goto call_transfer;
                        if (value < 0)
                            goto call_transfer;
                        goto primary;
                    }
                    arg = port;
                    if (value != 4)
                        goto call_transfer;
                    *count = three;
                    goto call_transfer;
                paired:
                    D_8009B744(other - 1);
                    result = 1;
                    goto save_count;
                primary:
                    {
                        register CardObj *base asm("$2") = D_8009B758;
                        other = (CardObj *)((u8 *)base + offset);
                    }
                    D_8009B744(other);
                    D_8009B748(other);
                    result = -1;
                save_count:
                    *count = result;
                }
            transfer:
                arg = port;
            call_transfer:
                result = MemCard_ReadByte(port, (u8)D_8009B72C(arg, mode));
                if (result < 0)
                    goto done;
                Timer_StartTimeout(60);
                if (!MemCard_WaitReadyForTransfer()) {
                timeout:
                    result = -3;
                    asm("" : : "r"(result));
                    goto done;
                }
                --D_8009B79C;
            } while (D_8009B79C >= 2);
        }
    }
    while (--D_8009B79C > 0) {
        result = MemCard_ReadByte(port, (u8)D_8009B72C(port, mode));
        if (result < 0)
            goto done;
        Timer_StartTimeout(60);
        {
            int ready = MemCard_WaitReadyForTransfer();
            if (!ready) {
                result = -3;
                goto done;
            }
        }
    }
    MemCard_WaitStatusBit2();
    port->response_3c[port->response_index++] = *D_8009B7A0;
    D_8009B724(0);
    result = 0;
done:
    return result;
}
