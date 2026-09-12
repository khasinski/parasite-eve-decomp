/* ASSEMBLER: GNU */
/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */
#include "pe1/bios_firstfile.h"

int strcmp(const char *, const char *);

/* Restore the first matching device's handler, then forward this request.
 * The handler used for restoration is saved before the name comparisons;
 * the callback used for forwarding is read again afterward. The two register
 * bindings and three empty barriers are included in debt. */
int Sys_FirstFileHookCallback(int *file, unsigned int arg1, unsigned int arg2) {
    register BiosDeviceEntry *entry;
    register BiosDeviceEntry *limit asm("$3");
    register BiosFirstFileHandler original;
    register unsigned int count asm("$3");
    register int *fileArg;

    if (!*file) *file = 1;
    count = *(unsigned int *)0x154;
    entry = *(BiosDeviceEntry **)0x150;
    original = D_800A32D0;
    count /= sizeof(BiosDeviceEntry);
    asm("" : "=r"(count) : "0"(count));
    limit = entry + count;
    asm("" : "=r"(limit), "=r"(file) : "0"(limit), "1"(file));
    fileArg = file;
    if (entry < limit) {
        register BiosDeviceEntry *end = limit;
        asm("" : "=r"(end) : "0"(end));
        do {
            if (entry->name && strcmp(entry->name, D_800A32D8) == 0) {
                entry->firstFile = original;
                break;
            }
            entry++;
        } while (entry < end);
        fileArg = file;
    }
    return D_800A32D0(fileArg, arg1, arg2);
}
