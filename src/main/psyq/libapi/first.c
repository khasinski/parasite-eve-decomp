/* ASSEMBLER: GNU */
/* Psy-Q LIBAPI FIRST.OBJ: firstfile, Sys_FirstFileHookCallback. */
#include "pe1/bios_firstfile.h"

/* SDK firstfile wrapper (FIRST.OBJ). Constraints are tracked in crutch debt.
 * Provenance: configs/USA/psyq_provenance.json (LIBAPI FIRST). */
int strcmp(const char *, const char *);
void *firstfile2(const char *, void *);
int Sys_FirstFileHookCallback(int *, unsigned, unsigned);
void *firstfile(const char *inName, void *inResult) {
    register const char *name = inName;
    register void *result = inResult;
    register const signed char *input;
    register char *out;
    register BiosDeviceEntry *entry;
    register BiosDeviceEntry *end;
    register unsigned count;
    register int found;
    asm("" : "=r"(name), "=r"(result) : "0"(name), "1"(result) : "memory");
    out = D_800A32D8;
    if (*(const signed char *)name >= 59) {
        input = (const signed char *)name;
        do {
            *out++ = *input++;
        } while (*input >= 59);
    }
    *out = 0;
    count = BIOS_DEVICE_DIRECTORY->byteLength;
    entry = BIOS_DEVICE_DIRECTORY->entries;
    count /= sizeof(BiosDeviceEntry);
    end = entry + count;
    if (entry < end) {
        register BiosDeviceEntry *limit = end;
        do {
            if (entry->name && strcmp(entry->name, D_800A32D8) == 0)
                goto save;
            entry++;
        } while (entry < limit);
    }
    found = 0;
check:
    if (found)
        goto second;
    return 0;
save:
    D_800A32D0 = entry->firstFile;
    found = 1;
    goto check;
install:
    entry->firstFile = Sys_FirstFileHookCallback;
    goto call;
second:
    count = BIOS_DEVICE_DIRECTORY->byteLength;
    entry = BIOS_DEVICE_DIRECTORY->entries;
    count /= sizeof(BiosDeviceEntry);
    end = entry + count;
    if (entry < end) {
        register BiosDeviceEntry *limit = end;
        do {
            if (entry->name && strcmp(entry->name, D_800A32D8) == 0)
                goto install;
            entry++;
        } while (entry < limit);
    }
call:
    return firstfile2(name, result);
}

/* Restore the first matching device's handler, then forward this request.
 * The handler used for restoration is saved before the name comparisons;
 * the callback used for forwarding is read again afterward. The empty
 * barriers are included in debt. */
int Sys_FirstFileHookCallback(int *file, unsigned int arg1, unsigned int arg2) {
    register BiosDeviceEntry *entry;
    BiosDeviceEntry *limit;
    register BiosFirstFileHandler original;
    unsigned int count;
    register int *fileArg;

    if (!*file) *file = 1;
    count = BIOS_DEVICE_DIRECTORY->byteLength;
    entry = BIOS_DEVICE_DIRECTORY->entries;
    original = D_800A32D0;
    count /= sizeof(BiosDeviceEntry);
    limit = entry + count;
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
