/* ASSEMBLER: GNU */
/* PSY-Q LIBPAD PADMAIN, part 6 of 8: _padInitSioMode, MemCard_RunCommandStep, _padSioRW. */
#include "pe1/psyq_pad_main.h"

extern int D_8009B764;

extern int D_8009B77C[];
extern void (*D_8009B744)(CardObj *), (*D_8009B748)(CardObj *);
int MemCard_WaitStatusBit2(void);
int _padInitSioMode(CardObj *port) {
    register MemCardSioRegisters *sio = D_8009B788;
    sio->control = 64;
    sio->control = 0;
    sio->mode = 13;
    sio->baud = 136;
    setRC2wait(port->field_e8 == 8 ? 80 : 145);
    {
        int index = D_8009B764;
        {
            register MemCardSioRegisters *regs = D_8009B788;
            register int control = 0x1003;
            if (index)
                control = 0x3003;
            regs->control = control;
        }
        {
            register int *address asm("$1") = D_8009B77C + index;
            register int remaining asm("$2") = *address;
            if (remaining >= 0) {
                if (remaining > 0) {
                    register int *counts = D_8009B77C;
                    do {
                        int *count = (int *)((D_8009B764 * 4) + (u32)counts);
                        (*count)--;
                        D_8009B744((CardObj *)port->field_0c + *count);
                    } while (counts[D_8009B764] > 0);
                }
                {
                    register int index = D_8009B764;
                    register int *table asm("$3");
                    register int *count;
                    table = D_8009B77C;
                    count = (int *)((index * 4) + (u32)table);
                    if (*count == 0) {
                        register int value asm("$3") = -1;
                        register CardObj *arg asm("$4") = port;
                        void (*callback)(CardObj *);
                        asm("" : : "r"(arg));
                        callback = D_8009B744;
                        *count = value;
                        callback(arg);
                        D_8009B748(port);
                    }
                }
            }
        }
    }
    sio = D_8009B788;
    if (sio->status & 0x200) {
        {
            register int control = sio->control;
            sio->control = control | 16;
        }
        if (sio->status & 0x200) {
            sio->data = 1;
            MemCard_WaitStatusBit2();
            (void)D_8009B788->data;
            return 0;
        }
        D_8009B784->status = ~128;
    }
    if (port->pad_50[0] && port->command)
        return 0;
    *port->response_3c = 0;
    return 1;
}

/* Adjacent PADMAIN.OBJ state-machine and SIO exchange functions.
 * Empty constraints are tracked in crutch debt. */
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
            setRC2wait(60);
            if (!MemCard_WaitReadyForTransfer())
                D_8009B724(-3);
        }
        if (D_8009B768 >= 5)
            D_8009B768--;
    } else
        D_8009B724(result);
}

s32 _padSioRW(CardObj *inObj, s32 inByte) {
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

        } while (chkRC2wait() == 0);
        result = ~outgoing;
        D_8009B788->data = result;
        result = initialByte;
        goto done;
    }
    {
        register int expected;
        {
            register u8 *response = port->response_3c;
            deviceId = *response;
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
            if (chkRC2wait() != 0) {
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
    return result;
}
