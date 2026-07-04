/* MASPSX_FLAGS: --store-branch-delay */
typedef int s32;

extern s32 D_80094564;
extern s32 D_80094568;
void DeliverEvent(unsigned int arg0, int arg1);

s32 Evt_Deliver(s32 arg0, s32 arg1) {
    D_80094564 = arg0;
    D_80094568 = arg1;

    if (arg0 == 0x21) {
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
