/* ASSEMBLER: GNU */
#include "common.h"
#include "pe1/psyq_cd.h"

extern char D_8001205C[];

int CdRom_IsBusy(u8 *dst, int sector_size);
int strncmp(char *s1, char *s2, int n);
void DsReadBreak(void);

void GD_disk_kind(int event, void *data, void *detail);

void GD_cbsync(unsigned char arg0) {
    if (arg0 == 2) {
        CdRom_InitAsyncRead(GD_disk_kind, 0);
    } else {
        g_DsDiskType = 2;
    }
}

void GD_disk_kind(int event, void *data, void *detail) {
    u8 arg0 = event;
    u8 buffer[8];
    /* Preserve the branch result in the strncmp return register. */
    register int disk_type asm("$2");

    if (arg0 == 1) {
        CdRom_IsBusy(buffer, 2);
        if ((disk_type = strncmp((char *)&buffer[1], D_8001205C, 5)) != 0) {
            disk_type = 2;
        } else {
            disk_type = 4;
        }
    } else {
        disk_type = 2;
    }

    g_DsDiskType = disk_type;
    DsReadBreak();
}
