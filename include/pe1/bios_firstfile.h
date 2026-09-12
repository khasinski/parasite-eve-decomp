#ifndef PE1_BIOS_FIRSTFILE_H
#define PE1_BIOS_FIRSTFILE_H

/* The hook forwards the file-state pointer and two unchanged argument words. */
typedef int (*BiosFirstFileHandler)(int *, unsigned int, unsigned int);

/* BIOS device-table entries reached through the pointers at 0x150/0x154.
 * Only the name and firstfile handler are recovered here; PSX size is 0x50. */
typedef struct BiosDeviceEntry {
    /* 0x00 */ char *name;
    /* 0x04 */ char reserved04[0x30];
    /* 0x34 */ BiosFirstFileHandler firstFile;
    /* 0x38 */ char reserved38[0x18];
} BiosDeviceEntry;

extern BiosFirstFileHandler D_800A32D0;
extern char D_800A32D8[];

#endif
