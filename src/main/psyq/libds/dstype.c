/* ASSEMBLER: GNU */
#include "pe1/psyq_ds_queue.h"
#include "pe1/cdrom.h"

extern char D_8001205C[];

CdlLOC *CdIntToPos(int sector, CdlLOC *position);
void GD_cbsync(unsigned char event);


int CdRom_IsBusy(u8 *dst, int sector_size);
void GD_disk_kind(int event, void *data, void *detail);

int DsGetDiskType(void) {
    CdlLOC pos;

    if (DsSync(0) == 2 && DsSync(1) == 16) {
        /* Prevent copying the known comparison value from v1 into v0. */
        asm volatile("" : : : "$3");
        return 16;
    }
    while (Cd_GetReadyStatus() != 1) {
        if (Cd_GetReadyStatus() == 3) {
            return 1;
        }
    }
    if (DsRead_IsBusy()) {
        DsReadBreak();
    }
    CdIntToPos(16, &pos);
    g_DsDiskType = 0;
    if (!Render_BuildParticleFrame(32, &pos, 27, (DslCB)GD_cbsync, 0)) {
        return 2;
    }
    while (!g_DsDiskType) {
    }
    return g_DsDiskType;
}

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
