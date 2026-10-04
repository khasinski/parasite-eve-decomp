#include "fx_common_motion.h"

/* Choose the room to return to once the effect sequence ends, clear the
 * screen, fade the music out and hand control back to the scene loader. */
void func_80192030(void)
{
    FxCommonRect area;
    s32 room;
    u32 night;
    u32 band;
    u8 fade;
    int i;

    fade = 1;
    night = g_FxCommonSceneIds.flags & 0x2000;
    g_FxCommonSceneIds.room = 999;
    D_8009D280 = 0xA9400048;
    room = g_FxCommonSceneIds.area;
    if (room == 0x80) {
        D_8009D280 = 0xA80650C8;
    } else if (room == 0x78) {
        D_8009D280 = 0xA8003348;
        fade = 0;
    } else if (room == 0x208) {
        D_8009D280 = 0xA80032C8;
        fade = 0;
    } else if (room == 0x210) {
        if (night)
            D_8009D280 = 0xA8029148;
        else
            D_8009D280 = 0xA80290C8;
    } else {
        switch (D_8019CA68) {
        case 0:
            D_8009D280 = 0xA80281C8;
            break;
        case 1:
            D_8009D280 = 0xA80260C8;
            break;
        case 2:
            D_8009D280 = 0xA8009348;
            break;
        case 3:
            D_8009D280 = 0xA8046048;
            break;
        case 4:
            if (room == 0x1C0) {
                D_8009D280 = 0xA80034C8;
                fade = 0;
            } else if (night) {
                D_8009D280 = 0xA80222C8;
            } else {
                D_8009D280 = 0xA80630C8;
            }
            break;
        case 5:
            D_8009D280 = 0xA8049048;
            break;
        case 6:
            D_8009D280 = 0xA8002248;
            break;
        case 7:
            if (room == 0xD0) {
                D_8009D280 = 0xA80033C8;
                fade = 0;
            } else if (room == 0x178) {
                D_8009D280 = 0xA80033C8;
                fade = 0;
            } else {
                band = room - 0x180;
                if (band < 0x88)
                    D_8009D280 = 0xA80201C8;
                else
                    D_8009D280 = 0xA8004348;
            }
            break;
        case 8:
            if (room == 0xE0) {
                D_8009D280 = 0xA80034C8;
                fade = 0;
            } else if (room >= 0x128) {
                D_8009D280 = 0xA8005448;
            } else {
                D_8009D280 = 0xA8067248;
            }
            break;
        case 9:
            if (room == 0xB8 || room == 0x148) {
                D_8009D280 = 0xA8003448;
                fade = 0;
            } else if (night) {
                D_8009D280 = 0xA8029148;
            } else {
                D_8009D280 = 0xA80290C8;
            }
            break;
        }
    }

    func_80074D28(0);
    func_80074A44(1);
    area.x = 0;
    area.y = 0;
    area.w = 0x140;
    area.h = 0x1C0;
    func_80074F44(&area, 0, 0, 0);
    func_80074DC0(0);
    func_800755F0(D_800BCE80);
    if (fade == 1)
        func_80086C5C(D_800B0DB5, 0x1E, 0);
    func_800868AC(0x1E, 0);
    for (i = 0; i < 0x1E; i++)
        func_80073A44(0);
    if (fade == 1) {
        D_800B0DB4 = -1;
        D_800B0DB2 = -1;
        g_FxCommonGameFlags &= ~0x40;
        func_80086FF8();
    }
    func_80087024();
    func_80038D48();
}

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
    FxCommonAddress current;

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
                if ((FxCommonPacketTag *)(D_8019C9C0->allocation[i].packed |
                                          0x80000000) ==
                    &D_8019C9C0->allocation[i - 1]) {
                    last = i;
                    scanning = 0;
                }
            } else if ((FxCommonPacketTag *)(D_8019C9C0->allocation[i].packed |
                                             0x80000000) !=
                       &D_8019C9C0->allocation[i - 1]) {
                current.pointer = &D_8019C9C0->allocation[i];
                D_8019C9C0->allocation[last].packed = current.word & 0xFFFFFF;
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
