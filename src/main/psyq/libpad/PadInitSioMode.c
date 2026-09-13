/* ASSEMBLER: GNU */
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
    Timer_StartTimeout(port->field_e8 == 8 ? 80 : 145);
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
                        int *count = (int *)((u32)(D_8009B764 * 4) + (u32)counts);
                        (*count)--;
                        D_8009B744((CardObj *)port->field_0c + *count);
                    } while (counts[D_8009B764] > 0);
                }
                {
                    register int index = D_8009B764;
                    register int *table asm("$3");
                    register int *count;
                    table = D_8009B77C;
                    count = (int *)((u32)(index * 4) + (u32)table);
                    if (*count == 0) {
                        register int value asm("$3") = -1;
                        register CardObj *arg asm("$4") = port;
                        register void (*callback)(CardObj *) asm("$2");
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
