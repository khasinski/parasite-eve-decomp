#include "fx_common_motion.h"

/* The effect overlay's own frame loop: run every effect system, compact the
 * ordering table, present the frame and flip the double buffer until the
 * scene asks to leave. */
void func_8019234C(void)
{
    FxCommonBuffer *next;
    int last;
    int handle;
    u8 started;
    int scanning;
    int i;

    D_8009CDDC = 0;
    D_8019C00E = 0;
    last = 0;
    func_80196498();
    func_80191DE8(D_8019C034 != 10);
    func_80071A64(func_80073A44(-1));
    started = 0;
    D_8019C03C = func_80071A54() % 3000000;
    handle = 0;
    if (D_8019C034 == 10)
        D_8019C03C = 120000;

    while (D_8019C024 == 0) {
        func_8003EB04();
        if ((D_8009D26C & 0x0F000006) == 0x0F000006) {
            func_80073A44(0);
            func_80074D28(0);
            D_8019C00E = 1;
            D_8019C024 = 1;
            func_8006A25C();
        }
        func_801942FC();
        if (D_8019C00E != 0)
            continue;

        func_8018F05C();
        func_8018F92C((void *)&g_FxCommonMotionPosition1);
        if (D_8019C02A != 0)
            D_8019C02A = func_80194108(D_8019C02A);
        func_80192740();
        func_80192800();
        func_80193478();
        if (!started) {
            handle = func_80191E30(0xABE, D_801EA578->state);
            started = 1;
        } else if (D_8019C0C0 == 0) {
            func_80191EFC(handle, D_801EA578->state);
        }
        if (D_8019C034 == 10 || (D_8019C044 == 1 && D_8019C045 == 0))
            func_80037870();

        scanning = 1;
        for (i = 0xFFF; i >= 0; i--) {
            if (scanning) {
                if ((D_8019C9C0->allocation[i].packed | 0x80000000) ==
                    (u32)&D_8019C9C0->allocation[i - 1]) {
                    last = i;
                    scanning = 0;
                }
            } else if ((D_8019C9C0->allocation[i].packed | 0x80000000) !=
                       (u32)&D_8019C9C0->allocation[i - 1]) {
                D_8019C9C0->allocation[last].packed =
                    (u32)&D_8019C9C0->allocation[i] & 0xFFFFFF;
                scanning = 1;
            }
        }

        D_8019CC14 = func_80073A44(1);
        func_80074DC0(0);
        func_80073A44(2);
        func_80193AB0();
        func_80074A44(1);
        func_80075424(((FxCommonFrame *)D_8019C9C0)->drawEnv);
        func_800755F0(((FxCommonFrame *)D_8019C9C0)->dispEnv);
        func_800753B4(&D_8019C9C0->allocation[0xFFF]);
        next = &g_FxCommonFrames[0].buffer;
        if (D_8019C9C0 == next)
            next = &g_FxCommonFrames[1].buffer;
        D_8019C9C0 = next;
        D_8009CDDC ^= 1;
        func_800752AC(next->allocation, 0x1000);
        func_8019BF8C((void **)D_8019C9C0);
    }
    if (D_8019C00E == 0)
        func_80192030();
}
