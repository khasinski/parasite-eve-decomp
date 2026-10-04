#include "common.h"
#include "pe1/memcard.h"

extern MemCardPortState D_800A0ED4[];
extern int D_800A1820;
extern int D_800A1824;
extern int D_800A1828;
extern int D_800A182C;
extern int D_800A1830;
extern int D_800A1834;
extern int D_800A1838;
extern unsigned int D_800A183C;
extern int D_800A1840;
extern int D_800BCDA8;
extern int D_800BCDAC;
extern int D_800BCDB0;
extern int D_800BCDB4;
extern int D_800BCDB8;
extern int D_800BCDBC;
extern int D_800BCDC0;
extern int D_800BCDC4;

int TestEvent(int event);
void MemCard_InitCardSlot(int arg0);
void _card_info(int arg0);
void _card_load(int arg0);
int Menu_IsMemCardDialogOpen(void);
void MemCard_StartRead(int port, int arg1);

void MemCard_StepPortState(int port) {
    MemCardPortState *state;
    int value;

    state = &D_800A0ED4[port];

    switch (state->cardState) {
    case 0:
        state->present = 0;
        break;

    case 1:
        if (D_800A1820 != 0) {
            D_800A1820 = 0;
            if ((state->present & 1) == 0) {
                TestEvent(D_800BCDB8);
                TestEvent(D_800BCDBC);
                TestEvent(D_800BCDC0);
                TestEvent(D_800BCDC4);
                D_800A1834 = 0;
                D_800A1830 = 0;
                D_800A182C = 0;
                MemCard_InitCardSlot(port << 4);
                state->cardState = 2;
                return;
            }
            state->cardState = 4;
            D_800A1840 = 0;
            D_800A183C = D_800A183C < 1;
            return;
        }
        if (D_800A1824 != 0) {
            D_800A1824 = 0;
            state->cardState = 0;
            state->present &= ~4;
            D_800A1840 = 0;
            D_800A183C = D_800A183C < 1;
            return;
        }
        if (D_800A1828 == 0) {
            return;
        }
        D_800A1828 = 0;
        TestEvent(D_800BCDB8);
        TestEvent(D_800BCDBC);
        TestEvent(D_800BCDC0);
        TestEvent(D_800BCDC4);
        D_800A1834 = 0;
        D_800A1830 = 0;
        D_800A182C = 0;
        MemCard_InitCardSlot(port << 4);
        state->cardState = 2;
        return;

    case 2:
        if (D_800A182C != 0) {
            D_800A182C = 0;
            TestEvent(D_800BCDA8);
            TestEvent(D_800BCDAC);
            TestEvent(D_800BCDB0);
            TestEvent(D_800BCDB4);
            D_800A1828 = 0;
            D_800A1824 = 0;
            D_800A1820 = 0;
            _card_load(port << 4);
            state->cardState = 3;
            return;
        }
        if (D_800A1830 == 0 && D_800A1834 == 0) {
            return;
        }
        D_800A1830 = 0;
        D_800A1834 = 0;
        state->cardState = 0;
        D_800A1840 = 0;
        D_800A183C = D_800A183C < 1;
        return;

    case 3:
        if (D_800A1820 != 0) {
            D_800A1820 = 0;
            state->cardState = 4;
            if (Menu_IsMemCardDialogOpen() != 0) {
                D_800A1840 = 0;
                D_800A183C = D_800A183C < 1;
                return;
            }
            state->present |= 1;
            MemCard_StartRead(port, 0);
            D_800A1840 = 0;
            D_800A183C = D_800A183C < 1;
            return;
        }
        if (D_800A1824 != 0) {
            D_800A1824 = 0;
            state->cardState = 0;
            D_800A1840 = 0;
            D_800A183C = D_800A183C < 1;
            return;
        }
        if (D_800A1828 == 0) {
            return;
        }
        D_800A1828 = 0;
        state->cardState = 4;
        state->present |= 4;
        D_800A1840 = 0;
        D_800A183C = D_800A183C < 1;
        return;

    case 4:
        state->present |= 1;
        break;

    default:
        return;
    }

    if (D_800A1838 != 0) {
        return;
    }
    if (port != D_800A183C) {
        return;
    }

    value = D_800A1840;
    D_800A1840 = value - 1;
    if (value > 0) {
        return;
    }

    TestEvent(D_800BCDA8);
    TestEvent(D_800BCDAC);
    TestEvent(D_800BCDB0);
    TestEvent(D_800BCDB4);
    D_800A1828 = 0;
    D_800A1824 = 0;
    D_800A1820 = 0;
    _card_info(port << 4);
    state->cardState = 1;
}

#include "common.h"
#include "pe1/memcard.h"
extern int D_800A1850;
extern int D_800BCDA8;
extern int D_800BCDAC;
extern int D_800BCDB0;
extern int D_800BCDB4;
extern int D_800BCDB8;
extern int D_800BCDBC;
extern int D_800BCDC0;
extern int D_800BCDC4;
extern MemCardPortState D_800A0ED4[];

int EnterCriticalSection(void);
int ExitCriticalSection(void);
int OpenEvent(int desc, int spec, int mode, void (*func)(void));
int EnableEvent(int event);
void MemCard_InitCardSubsystem(int arg0);
long StartCARD(void);
void _bu_init(void);
void _card_auto(int arg0);

void MemCard_OnEventF400Spec0004(void);
void MemCard_OnEventF400Spec8000(void);
void MemCard_OnEventF400Spec0100(void);
void MemCard_OnEventF400Spec2000(void);
void MemCard_OnEventF000Spec0004(void);
void MemCard_OnEventF000Spec8000(void);
void MemCard_OnEventF000Spec0100(void);
void MemCard_OnEventF000Spec2000(void);

void MemCard_InitManager(void) {
    int i;
    int *events;
    int value;
    int event;
    register int desc asm("$4");
    register int spec asm("$5");
    register int mode asm("$6");
    void (*callback)(void);
    if (D_800A1850 == 0) {
        D_800A1850 = 1;
        i = 0;
        EnterCriticalSection();

        event = OpenEvent(0xF4000001, 0x0004, 0x1000, MemCard_OnEventF400Spec0004);
        desc = 0xF4000001;
        spec = 0x8000;
        mode = 0x1000;
        callback = MemCard_OnEventF400Spec8000;
        /* Keep the event table base load after the second OpenEvent arguments. */
        asm volatile("" ::: "$16");
        events = &D_800BCDA8;
        events[0] = event;
        D_800BCDAC = OpenEvent(desc, spec, mode, callback);
        D_800BCDB0 = OpenEvent(0xF4000001, 0x0100, 0x1000, MemCard_OnEventF400Spec0100);
        D_800BCDB4 = OpenEvent(0xF4000001, 0x2000, 0x1000, MemCard_OnEventF400Spec2000);
        D_800BCDB8 = OpenEvent(0xF0000011, 0x0004, 0x1000, MemCard_OnEventF000Spec0004);
        D_800BCDBC = OpenEvent(0xF0000011, 0x8000, 0x1000, MemCard_OnEventF000Spec8000);
        D_800BCDC0 = OpenEvent(0xF0000011, 0x0100, 0x1000, MemCard_OnEventF000Spec0100);
        D_800BCDC4 = OpenEvent(0xF0000011, 0x2000, 0x1000, MemCard_OnEventF000Spec2000);

        MemCard_InitCardSubsystem(0);
        StartCARD();
        _bu_init();
        _card_auto(0);

        do {
            int loop_event;
            loop_event = *events;
            events++;
            /* Keep the table pointer increment before the EnableEvent call. */
                        EnableEvent(loop_event);
            i++;
        } while (i < 8);

        ExitCriticalSection();
    }

    value = 0x418;
    do {
        ((u8 *)D_800A0ED4)[value] = 0;
        value -= 0x418;
    } while (value >= 0);
}
