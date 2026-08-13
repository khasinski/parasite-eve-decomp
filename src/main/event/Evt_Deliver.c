/* CC1_FLAGS: -O1 */
#include "pe1/event.h"

extern s32 D_80094564;
typedef struct EvtDeliverDataPage {
    u8 pad_0000[0x4568];
    s32 value;
} EvtDeliverDataPage;

register EvtDeliverDataPage *g_EvtDeliverDataPage asm("$1");
void DeliverEvent(unsigned int arg0, int arg1);

s32 Evt_Deliver(s32 arg0, s32 arg1) {
    register s32 event301 asm("$2") = 0x21;

    D_80094564 = arg0;
    g_EvtDeliverDataPage = (EvtDeliverDataPage *)0x80090000;
    g_EvtDeliverDataPage->value = arg1;

    if (arg0 == event301) {
        goto event301;
    }
    if (arg0 == 0x22) {
        goto event302;
    }
    goto done;

event301:
    DeliverEvent(0xF4000002, 0x301);
    goto done;

event302:
    DeliverEvent(0xF4000002, 0x302);

done:
    return 0;
}
