/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */
#include "pe1/psyq_pad_main.h"
#include "pe1/psyq_nop.h"

int MemCard_WriteByte(CardObj *obj, int value)
{
    MemCardSioRegisters *sio;
    MemCardInterruptRegisters *irq;
    volatile u16 *counter;
    volatile u16 *target;
    volatile u16 *mode;
    u32 start;
    u32 limit;
    u32 ticks;
    u32 wrap;
    int baud;
    int received;
    int first_byte;
    u16 poll_status;

    first_byte = obj->response_3c[0];
    baud = 0x88;
    if ((first_byte >> 4) == 8 && obj->response_index >= 9)
        baud = 0x22;

    {
        MemCardSioRegisters *ready_sio = D_8009B788;
        PE1_NOP();
        do {
            poll_status = ready_sio->status;
        } while ((poll_status & 2) == 0);
    }
    Timer_StartTimeout(400);

    sio = D_8009B788;
    received = sio->data;
    if (obj->response_index == 0 && (received >> 4) == 8)
        goto special_baud;
    sio->baud = baud;
    goto baud_done;
special_baud:
    sio->baud = 0x22;
baud_done:

    irq = D_8009B784;
    if (!(irq->status & 0x80)) {
        register MemCardInterruptRegisters *poll_irq;
        counter = (volatile u16 *)0x1F801120;
        target = (volatile u16 *)0x1F801128;
        wrap = 0x10000;
        mode = (volatile u16 *)0x1F801124;
        poll_irq = irq;
        start = D_800A76D0;
        limit = D_800BD02C;
        do {
            ticks = *counter;
            if (ticks < start) {
                if (*target != 0)
                    ticks += *target;
                else
                    ticks += wrap;
            }
            if (*mode & 0x200) {
                if (ticks - start >= limit)
                    return -2;
            }
            if (((ticks - start) >> 3) >= limit)
                return -2;
        } while (!(poll_irq->status & 0x80));
    }

    if (obj->field_e8 != 8 && D_8009B768 == 2) {
        Timer_StartTimeout(60);
        while (!Spu_CheckTimerElapsed()) { }
    }

    D_8009B788->data = value;
    obj->payload_index++;
    if (obj->response_index != 0xFF) {
        /* The response cursor is read again after the payload update. */
        asm volatile("" ::: "memory");
        obj->response_3c[obj->response_index] = received;
    }
    obj->response_index++;
    return received;
}
