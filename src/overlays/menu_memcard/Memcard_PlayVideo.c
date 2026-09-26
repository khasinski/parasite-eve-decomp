#include "menu_memcard_video.h"

s32 Memcard_PlayVideo(s32 index) {
    u8 *buffer;
    s32 entry = index;
    s32 result = 0;
    register u32 *flags asm("$3");
    s32 ready;
    flags=&D_800B0CD8;
    *flags |= 0x200;
    D_801D0E00[entry].enabled=1;
    SetDispMask(0); DrawSync(0); func_80074A44(1);
retry:
    {
        VideoDiscRange *range = &D_8009315E;
        s32 readError = -1;
        s32 pollError;
        do {} while(func_8006E6A8(D_800B0DD8+range->start,D_8001160C,range->end-range->start)==readError);
        pollError=-1;
        for (;;) {
            ready=CdRom_PollReady();
            if (ready==0) break;
            if (ready==pollError) goto retry;
        }
    }
    func_80072714(); func_800726C4(); func_80072724();
    buffer=D_80011610+((D_80093162-D_80093160)<<11);
    Memcard_InitVideoBuffers(1,&buffer);
    func_801924F8(entry);
    {
        u8 *active=&D_800B0DBA;
        if (*active) {
            s16 *remainingPtr=(s16 *)(active+2);
            do {
                s32 left = *remainingPtr;
                if (left<=0) break;
                func_8003EB04();
                if (!(s8)func_80192934()) {
                    D_801D0DE8=0;D_801D0DEC=0;D_801D0DFC=0;D_801D0DF8=0;D_801D0DF0=0;D_801D0DF4=0;
                    D_800B0DBA=0;
                }
                else if (D_8009D26C & 0x20000004) {
                    s32 remaining;
                    D_800B0DBA--;
                    func_800870F0(0);func_8010C0D8(0);func_8007A2A4();func_80080DC4(9,0,0);
                    remaining=*remainingPtr;
                    D_801D0DE8=0;D_801D0DEC=0;D_801D0DFC=0;D_801D0DF8=0;D_801D0DF0=0;D_801D0DF4=0;
                    D_800B0DBA=0;
                    if(remaining<1400) {VSync(0);SetDispMask(0);result=1;}
                }
                Gpu_RenderFrame();
            } while(D_800B0DBA);
        }
    }
    { register s32 returnValue asm("$2") = result;
        flags=&D_800B0CD8;
        *flags &= ~0x200;
        return returnValue; }
}
