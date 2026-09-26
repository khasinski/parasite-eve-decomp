#include "pe1/boot_display.h"

/* Retry labels preserve the original command-error control flow. */
s32 Boot_StartPlayback(void) {
    u8 mode[8];
    u8 filter[8];
    u8 result[8];
    s32 request;
    s32 status;
    mode[0] = 200;
retryMode:
    do {
        while (func_8007F72C() != 1) {}
    } while (func_8007F778() != 0);
    request = func_8007EE84(14, mode, 0, -1);
    if (request == 0) goto retryMode;
    while ((status = func_8007F418(request, result)) != 2) {
        if (status != 0) goto retryMode;
    }
    filter[0] = 1;
    filter[1] = 0;
retryFilter:
    do {
        while (func_8007F72C() != 1) {}
    } while (func_8007F778() != 0);
    request = func_8007EE84(13, filter, 0, -1);
    if (request == 0) goto retryFilter;
    while ((status = func_8007F418(request, result)) != 2) {
        if (status != 0) goto retryFilter;
    }
    func_800824C8(Boot_StepPlaybackTransfer);
    func_80073D24(Boot_CheckPlaybackTime);
    func_801258EC(200, 27);
    do {
        while (func_8007F72C() != 1) {}
    } while (func_8007F778() != 0);
    return 0;
}
