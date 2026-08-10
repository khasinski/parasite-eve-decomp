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
