/* ASSEMBLER: GNU */
/* SDK firstfile wrapper (FIRST.OBJ). Constraints are tracked in crutch debt.
 * See proposals/PsyqPadFirstfile for provenance and full-byte validation. */
#include "pe1/bios_firstfile.h"
int strcmp(const char *, const char *);
void *firstfile(const char *, void *);
int Sys_FirstFileHookCallback(int *, unsigned, unsigned);
void *Scene_CreateEntityNode(const char *inName, void *inResult) {
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
    count = *(unsigned *)0x154;
    entry = *(BiosDeviceEntry **)0x150;
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
    count = *(unsigned *)0x154;
    entry = *(BiosDeviceEntry **)0x150;
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
    return firstfile(name, result);
}
