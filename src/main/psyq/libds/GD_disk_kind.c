#include "common.h"

extern char D_8001205C[];
extern int g_DsDiskType;

int CdRom_IsBusy(u8 *dst, int sector_size);
int strncmp(char *s1, char *s2, int n);
void DsReadBreak(void);

void GD_disk_kind(u8 arg0) {
    u8 buffer[8];
    int disk_type;

    if (arg0 == 1) {
        CdRom_IsBusy(buffer, 2);
        disk_type = strncmp((char *)&buffer[1], D_8001205C, 5);
        if (disk_type == 0) {
            disk_type = 4;
        } else {
            disk_type = 2;
        }
    } else {
        disk_type = 2;
    }

    g_DsDiskType = disk_type;
    DsReadBreak();
}
