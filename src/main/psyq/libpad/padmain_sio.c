/* ASSEMBLER: GNU */
/* Adjacent PADMAIN.OBJ state-machine and SIO exchange functions.
 * Empty constraints are tracked; see proposals/PsyqPadFirstfile. */
#include "pe1/psyq_pad_main.h"
void MemCard_RunCommandStep(void) {
    register int *index asm("$5") = &D_8009B768;
    register int count;
    register int result;
    register int (*next)(void) asm("$2");
    count = *index;
    next = D_8009B7A8[count];
    *index = count + 1;
    result = next();
    if (result >= 0) {
        if (D_8009B768) {
            Timer_StartTimeout(60);
            if (!MemCard_WaitReadyForTransfer())
                D_8009B724(-3);
        }
        if (D_8009B768 >= 5)
            D_8009B768--;
    } else
        D_8009B724(result);
}

s32 MemCard_ReadByte(CardObj *inObj, s32 inByte) {
    register CardObj *port = inObj;
    register s32 outgoing = inByte;
    register s32 received asm("$4");
    register s32 deviceId;
    register s32 initialByte asm("$17");
    register u32 baud;
    register s32 replyByte asm("$17");
    register s32 result asm("$2");

    if (outgoing < 0) {
        received = D_8009B788->data;
        asm("" : "=r"(received) : "0"(received));
        port->response_index = 0xFF;
        port->payload_index = 1;
        *port->field_40 = ~outgoing;
        initialByte = received & 0xFF;
        if (!(D_8009B788->status & 1)) {
            do {

            } while (!(D_8009B788->status & 1));
        }
        do {

        } while (Spu_CheckTimerElapsed() == 0);
        result = ~outgoing;
        D_8009B788->data = result;
        result = initialByte;
        goto done;
    }
    {
        register int expected;
        {
            register u8 *response = port->response_3c;
            asm("" : "=r"(response) : "0"(response) : "$3");
            deviceId = *response;
            asm("" : "=r"(deviceId) : "0"(deviceId) : "$3");
        }
        expected = 8;
        baud = 0x88;
        if ((deviceId >> 4) == expected) {
            if ((u8)port->response_index >= 9U) {
                baud = 0x22;
            }
        }
    }
    {
        register MemCardSioRegisters *registers = D_8009B788;
        register u32 timer = *(volatile u16 *)0x1F801120;
        register u32 ready = registers->status;
        register MemCardSioRegisters *cursor;
        D_800BD02C = 0x1AE;
        D_800A76D0 = timer;
        if (!(ready & 2)) {
            cursor = registers;
            do {
            } while (!(cursor->status & 2));
        } else
            cursor = registers;
    }
    {
        register MemCardSioRegisters *sio = D_8009B788;
        register MemCardInterruptRegisters *irq = D_8009B784;
        asm("" : "=r"(sio), "=r"(irq) : "0"(sio), "1"(irq));
        received = sio->data;
        asm("" : "=r"(received) : "0"(received));
        replyByte = received & 0xFF;
        sio->baud = baud;
        if (!(irq->status & 0x80)) {
        waitAcknowledge:
            if (Spu_CheckTimerElapsed() != 0) {
                result = -20;
                goto done;
            }
            {
                if (D_8009B784->status & 0x80) {
                    goto writeByte;
                }
                goto waitAcknowledge;
            }
        } else {
        writeByte:
            D_8009B788->data = (u8)outgoing;
            port->payload_index += 1;
            port->response_3c[port->response_index] = replyByte;
            result = port->response_index;
            result++;
            port->response_index = result;
            result = replyByte;
        }
    }
done:
    return (s32)result;
}
