#include "pe1/psyq_cd.h"
#include "pe1/pe_image.h"

extern char D_80011330[];
extern char D_80011348[];
extern char D_80011354[];
int Cd_GetReadyStatus(void);
int CdRom_GetPendingReadCount(void);
int DsGetDiskType(void);
int CdPosToInt(CdlLOC *position);
int VSync(int mode);

int OpenPeImage(void)
{
    DslFILE file;
    int status;
    int disk_type;
    DslFILE *found;

    g_PeImageMountFlags = 0;
    status = Cd_GetReadyStatus();
    if (status != 1) return 1;
    if (CdRom_GetPendingReadCount() != 0) return 1;
    disk_type = DsGetDiskType();
    if (disk_type == status) return 1;
    if (disk_type != 4) return -1;

    while (Cd_GetReadyStatus() != 1 || CdRom_GetPendingReadCount() != 0)
        VSync(0);
    found = DsSearchFile(&file, D_80011330);
    if (found != 0 && found != (DslFILE *)-1) {
        while (Cd_GetReadyStatus() != 1 || CdRom_GetPendingReadCount() != 0)
            VSync(0);
        found = DsSearchFile(&file, D_80011348);
        if (found != 0 && found != (DslFILE *)-1) {
            g_PeImageBaseLba = CdPosToInt(&file.pos);
            g_PeImageMountFlags = (u8)g_PeImageMountFlags | 1;
        }
    }
    while (Cd_GetReadyStatus() != 1 || CdRom_GetPendingReadCount() != 0)
        VSync(0);
    found = DsSearchFile(&file, D_80011354);
    if (found == 0) goto done;
    asm volatile("" : : "r"(found));
    if (found == (DslFILE *)-1) goto done;
    while (Cd_GetReadyStatus() != 1 || CdRom_GetPendingReadCount() != 0)
        VSync(0);
    found = DsSearchFile(&file, D_80011348);
    if (found == 0) goto done;
    asm volatile("" : : "r"(found));
    if (found == (DslFILE *)-1) goto done;
    g_PeImageBaseLba = CdPosToInt(&file.pos);
    g_PeImageMountFlags = (u8)g_PeImageMountFlags | 2;
done:
    return ((s8)g_PeImageMountFlags == 0) ? -2 : 0;
}
