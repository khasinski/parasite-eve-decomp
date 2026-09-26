#include "menu_memcard_video.h"

void *Memcard_UpdateVideoFrame(VideoDisplay *display) {
    struct { void *data; VideoFrame *frame; VideoRect clear; } local;
    register void **dataPtr asm("$4");
    s32 retries = 2000;
    s32 remaining;
    s32 end;
    s32 status;
    status = func_8007F72C();
    dataPtr = &local.data;
    if (status == 1) {
        status = func_8007F7A8();
        dataPtr = &local.data;
        if (status != D_800B0DD4) func_800719E4(1);
    }
    while (func_8007C484(dataPtr,&local.frame) != 0) {
        dataPtr = &local.data;
        if (--retries == 0) return 0;
    }
    end = D_801D11AC->end;
    if (local.frame->time >= (u32)(end - 16)) {
        s32 position = local.frame->time + 16;

        remaining = position - end;

        remaining = 14 - remaining;
        if (remaining < 0) remaining = 0;
        func_800870F0(D_800B0DBE * remaining / 14);
    }
    if (local.frame->time < (u32)D_801D11B0 || local.frame->time >= (u32)D_801D11AC->end) D_801D0DBD = 1;
    D_801D11B0 = local.frame->time;
    if (D_801D0DE0[0] != local.frame->width || D_801D0DE0[1] != local.frame->height) {
        register s32 height asm("$2");
        if (D_800B0DBB) { height=480;

            local.clear.x=0; local.clear.y=0; local.clear.w=height; }
        else { register s32 width asm("$2") = 320;

            local.clear.w=width; height=480;

            local.clear.x=0; local.clear.y=0; }
        local.clear.h=height;
        __asm__("" : : : "memory");
        func_80074F44(&local.clear,0,0,0);
        D_801D0DE0[0]=local.frame->width; D_801D0DE0[1]=local.frame->height;
    }
    if (D_800B0DBB) { s32 width = D_801D0DE0[0]*3; display->width22=(u32)(width + ((u32)width >> 31)) >> 1; display->width1a=display->width22; }
    else { display->width22=D_801D0DE0[0]; display->width1a=display->width22; }
    {
        volatile s16 *height = &D_801D0DE0[1];
        register u16 value asm("$3");
        void *result;

        value = *height;
        result = local.data;

        display->height24=value; display->height1c=value;
        value = *height;
        display->height2e=value;
        return result;
    }
}
