#include "common.h"

extern char D_8001205C[];

int CdRom_IsBusy(u8 *dst, int sector_size);
int strncmp(char *s1, char *s2, int n);
void DsReadBreak(void);

typedef struct CdDiskKindPage {
    char reserved00[0x28F8];
    int disk_type;
} CdDiskKindPage;

register CdDiskKindPage *g_CdDiskKindPage asm("$1");

void GD_disk_kind(u8 arg0) {
    u8 buffer[8];
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

    g_CdDiskKindPage = (CdDiskKindPage *)0x800B0000;
    g_CdDiskKindPage->disk_type = disk_type;
    DsReadBreak();
}
