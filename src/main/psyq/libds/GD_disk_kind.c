/* MASPSX_FLAGS: --store-branch-delay --store-call-delay */

typedef unsigned char u8;

extern int D_800B28F8;
extern char D_8001205C[];

int CdRom_IsBusy(u8 *dst, int sector_size);
int strncmp(char *s1, char *s2, int n);
void DsReadBreak(void);

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

    D_800B28F8 = disk_type;
    DsReadBreak();
}
