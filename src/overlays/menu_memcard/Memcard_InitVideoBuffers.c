#include "menu_memcard_video.h"

s32 Memcard_InitVideoBuffers(s8 count, u8 **buffers) {
    s8 i;
    VideoRect rectangle;
    if ((u8)(count - 1) >= 2) return 0;
    for (i=0; i<count; i++) if (!buffers[i]) return 0;
    if (D_800B0DBA) return 0;
    { s32 mode = count;
        switch(mode) {
            case 1: {
                u8 *base = buffers[0];
                register u8 *second asm("$5");
                register u8 *third asm("$4");
                register u8 *fourth asm("$3");
                register u8 *fifth asm("$2");
                register s32 offset asm("$6");
                offset = 0xFA00;
                second=base+offset;
                offset=0x1F400;
                third=base+offset;
                offset=0x3F400;
                fourth=base+offset;
                offset=0x50400;
                fifth=base+offset;
                offset=0x53100;
                D_801D0DF0=fifth; D_801D0DE8=base; D_801D0DEC=second;
                D_801D0DFC=third; D_801D0DF8=fourth;
                fifth=base+offset;
                D_801D0DF4=fifth;
                break;
            }
            case 2: {
                register u8 *base asm("$7") = buffers[0];
                u8 *second;
                register u8 *third asm("$2");
                register s32 offset asm("$6") = 0xFA00;
                second=base+offset;
                offset=0x1F400;
                third=base+offset;
                offset=0x10000;
                __asm__("" : "=r"(offset) : "0"(offset), "r"(third));
                D_801D0DE8=base; D_801D0DEC=second; D_801D0DFC=third;
                __asm__("" : : : "memory");
                base=buffers[1];
                offset |= 0x1000;
                third=base+offset;
                offset=0x13D00;
                D_801D0DF0=third; D_801D0DF8=base; third=base+offset;
                D_801D0DF4=third;
                break;
            }
        }
    }
    D_800B0DBA=1; D_800B0DBE=152; D_800B0DBC=0; D_801D11B0=-1;
    for(i=0;i<2;i++) { D_801D1384[i]=D_800BCE80[i]; D_801D13AC[i]=D_800BCDC8[i]; }
    rectangle.x=320; rectangle.y=0; rectangle.w=192; rectangle.h=256;
    func_8007512C(&rectangle,512,0);
    if (!(D_800B0CD8 & 0x08000000)) {
        rectangle.x=0; rectangle.y=448; rectangle.w=320; rectangle.h=64;
        func_8007512C(&rectangle,512,256);
    }
    return 1;
}
