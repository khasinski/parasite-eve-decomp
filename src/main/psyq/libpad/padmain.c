/* ASSEMBLER: GNU */
/* Psy-Q LIBPAD PADMAIN.OBJ: PadEnableCom, _padSetVsyncParam, _padChkVsync, _padStartCom, _padStopCom, _padInitSioMode, _padSioRW and their local helpers. */
#include "common.h"
#include "pe1/psyq_bios.h"
#include "pe1/psyq_nop.h"
#include "pe1/psyq_pad_main.h"

extern int g_MemCardPort1Present;
extern int g_MemCardPort2Present;
extern int g_MemCardServiceReady;
extern void (*g_MemCardObjResetFn)(CardObj *obj);

int PadEnableCom(int portMask) {
    int currentMask;
    register int timerValue asm("$2");
    register int *timer1 asm("$18");
    int *timer2;
    currentMask = (g_MemCardPort2Present << 1)
        | (g_MemCardPort1Present == 0);
    if (currentMask != portMask) {
        g_MemCardServiceReady = 0;

        if (portMask & 1) {
            timer1 = &D_800A5AC0[0];
            timerValue = *timer1;
            g_MemCardPort1Present = 0;
            if (timerValue >= 150) {
                g_MemCardObjResetFn(g_MemCardObjArray);
            }
            *timer1 = 0;
        } else {
            g_MemCardPort1Present = 1;
        }

        if (portMask & 2) {
            timer2 = &D_800A5AC0[1];
            timerValue = *timer2;
            g_MemCardPort2Present = 1;
            if (timerValue >= 150) {
                g_MemCardObjResetFn(g_MemCardObjArray + 1);
            }
            *timer2 = 0;
        } else {
            g_MemCardPort2Present = 0;
        }

        g_MemCardServiceReady = 1;
    }
    return currentMask;
}

int MemCard_TimerCallback(void);
int MemCard_TimerReadyCallback(void);
extern void *D_800A5AB4[];

void _padSetVsyncParam(void) {
    void **table;

    table = D_800A5AB4;
    asm volatile("" : "=r"(table) : "0"(table));
    table[0] = MemCard_TimerCallback;
    table[1] = MemCard_TimerReadyCallback;
    table[-1] = 0;
    table[2] = 0;
}

extern int (*D_8009B74C)(void);
typedef int (*MemCardStepFn)(void);
typedef void (*MemCardErrorFn)(int);

extern int g_MemCardCallbackPending;
extern int D_8009B75C;
extern int D_8009B764;
extern int D_8009B768;
extern int D_8009B76C;
extern int D_8009B774;
extern int D_8009B778;
extern int D_8009B78C;
extern MemCardStepFn D_8009B7A8[];
extern MemCardErrorFn D_8009B724;

void SysDeqIntRP(int index, void *queue);
void SysEnqIntRP(int index, void *queue);
extern int g_MemCardCounterIrqQueueNode;

int chkRC2wait(void);
int _padInitSioMode(CardObj *port);

int MemCard_TimerReadyCallback(void) {
    MemCardInterruptRegisters *state = g_MemCardState;

    if ((state->mask & 1) == 0) {
        return 0;
    }
    if ((state->status & 1) == 0) {
        return 0;
    }
    if (D_8009B74C != 0) {
        D_8009B74C();
    }
    return 1;
}

int MemCard_TimerCallback(void) {
    int index;
    int limit;
    int *timer;
    void *obj;
    int active;
    int one;

    active = D_8009B774;
    one = 1;
    g_MemCardCallbackPending = one;
    if (active != 0) {
        timer = &D_800A5AC0[0];
        active = *timer;
        if (active < 0x96) {
            one = active + 1;
            *timer = one;
        }
    }

    if (D_8009B778 == 0) {
        timer = &D_800A5AC0[1];
        active = *timer;
        if (active < 0x96) {
            one = active + 1;
            *timer = one;
        }
    }

    if (D_8009B75C != 0) {
        index = D_8009B774;
        limit = D_8009B778;
        if (limit >= index) {
            obj = (void *)((int)D_8009B758 + (((index << 4) - index) << 4));
            D_8009B768 = 0;
            D_8009B764 = index;
            if (_padInitSioMode(obj) == 0) {
                D_8009B724(0xFFFF);
            }

            D_8009B76C = 0;
            while (D_8009B778 >= D_8009B764) {
                index = D_8009B764;
                obj = (void *)((int)D_8009B758 + (((index << 4) - index) << 4));
                MemCard_RunCommandStep(obj);
            }
            g_MemCardSioRegs->baud = 0x88;
        }
    }

    return 0;
}

int _padChkVsync(void) {
    int old = g_MemCardCallbackPending;

    g_MemCardCallbackPending = 0;
    return old;
}

/* SDK _padStartCom (PADMAIN.OBJ). Constraints are tracked in crutch debt.
 * Provenance: configs/USA/psyq_provenance.json (LIBPAD PADMAIN). */
void _padStartCom(void) {
    D_8009B75C = 0;
    EnterCriticalSection();
    SysDeqIntRP(2, D_800A5AB0);
    SysEnqIntRP(2, D_800A5AB0);
    D_8009B784->status = -2;
    D_8009B784->mask |= 1;
    ChangeClearRCnt(3, 0);
    ExitCriticalSection();
    D_8009B728(D_8009B758);
    D_8009B728(D_8009B758 + 1);
    {
        register int *clear = D_800A5AC0;
        asm("" : "=r"(clear) : "0"(clear));
        clear[1] = 0;
        clear[0] = 0;
    }
    D_8009B75C = 1;
    return;
}

void _padStopCom(void) {
    EnterCriticalSection();
    ChangeClearRCnt(3, 1);
    SysDeqIntRP(2, D_800A5AB0);
    ExitCriticalSection();
}

extern int D_8009B77C[];
extern void (*D_8009B744)(CardObj *), (*D_8009B748)(CardObj *);
void MemCard_WaitStatusBit2(void);
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
void MemCard_RunCommandStep(CardObj *port) {
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
    register u32 baud;
    s32 replyByte;
    register s32 result asm("$2");

    if (outgoing < 0) {
        received = D_8009B788->data;
        asm("" : "=r"(received) : "0"(received));
        port->response_index = 0xFF;
        port->payload_index = 1;
        *port->field_40 = ~outgoing;
        replyByte = received & 0xFF;
        if (!(D_8009B788->status & 1)) {
            do {

            } while (!(D_8009B788->status & 1));
        }
        do {

        } while (chkRC2wait() == 0);
        result = ~outgoing;
        D_8009B788->data = result;
        result = replyByte;
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
    setRC2wait(400);

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
        setRC2wait(60);
        while (!chkRC2wait()) { }
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

int MemCard_WaitReadyForTransfer(void)
{
  register MemCardInterruptRegisters *state;
  MemCardSioRegisters *regs;
  MemCardSioRegisters *check_regs;
  int value;
  unsigned char status;
  state = g_MemCardState;
  regs = g_MemCardSioRegs;
  value = -0x81;
  state->status = value;
  status = regs->status;
  status &= 0x80;
  while (status != 0)
  {
    if (chkRC2wait() != 0)
    {
      return 0;
    }
    check_regs = g_MemCardSioRegs;
    status = check_regs->status;
    status &= 0x80;
  }
  regs = g_MemCardSioRegs;

  regs->control |= 0x10;
  return 1;
}

void MemCard_WaitStatusBit2(void) {
    MemCardSioRegisters *ptr = g_MemCardSioRegs;

    /* Keep the poll loop target on the lhu, not the load-delay nop. */
    PE1_NOP();
    while ((ptr->status & 2) == 0) {
    }
}
