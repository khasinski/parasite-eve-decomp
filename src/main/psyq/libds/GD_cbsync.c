extern char GD_disk_kind[];
extern int g_DsDiskType;

void CdRom_InitAsyncRead(void *arg0, int arg1);

void GD_cbsync(unsigned char arg0) {
    if (arg0 == 2) {
        CdRom_InitAsyncRead(GD_disk_kind, 0);
    } else {
        g_DsDiskType = 2;
    }
}
