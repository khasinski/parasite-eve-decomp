#include "pe1/boot_display.h"
#include "pe1/psyq_cd.h"
#include "pe1/psyq_ds_queue.h"

s32 Boot_InitPlaybackFile(void) {
    DslFILE file;
    s32 found;
    /* Preserve separate zero/-1 retries and the ready-constant reload.
     * Pins and empty barriers are tracked in crutch debt. */
    register s32 ready asm("$16");
    register s32 failure asm("$2");
    volatile s32 *start;
    func_8007EC14();
    func_80080CC8(0);
    for (;;) {
        ready = 1;
        asm("" : "=r"(ready) : "0"(ready));
        if (func_8007F72C() != ready || func_8007F778() != 0)
            return func_80073A44(0);
        found = (s32)DsSearchFile(&file, D_80125B48);
        if (found == 0) continue;
        failure = -1;
        asm("" : "=r"(failure) : "0"(failure));
        asm volatile("" : "=r"(ready) : "0"(ready));
        if (found != failure) break;
    }
    start = &D_80172CE8;
    *start = CdPosToInt(&file.pos);
    D_80172CEC = CdPosToInt(&file.pos) + (file.size >> 11);
    D_80172CF0 = *start;
    D_80172CF4 = 0;
    D_80172CF8 = 0;
    D_80172CFA = 0;
    D_80172D00 = D_80011610 + (((D_80093168 - D_80093166) << 11) + 0x34800);
    return 0;
}
