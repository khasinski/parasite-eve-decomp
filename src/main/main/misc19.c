#include "include_asm.h"

/* CC1_PROFILE: build0001 */

typedef unsigned short u16;

int VSync(int arg0);
void SetDispMask(int arg0);
int CD_ReadSectors(int arg0, int arg1, int arg2, int arg3, int arg4, int arg5);
int CdRom_ReadSectors(int lba, int offset, int dst, int size);
int Sys_VSyncTimeout(int arg0);
int Gpu_LoadTimImage(int arg0);
int DrawSync(int arg0);
void EnterCriticalSection(void);
void FlushCache(void);
void ExitCriticalSection(void);

extern int g_PeImageBaseLba;
extern int g_SceneLoadScratchBuffer;
extern int D_800A77FC;
extern int g_StrFileDirBuffer;
extern int g_GameState[];
extern int D_800B0CD8_word __asm__("g_GameState");
extern u16 g_MainAssetLbaTbl[];
extern u16 g_LargeTexLbaTbl[];
extern u16 D_8009316C[];
extern u16 g_FontUiLbaTbl[];

typedef unsigned char u8;

extern char *g_PmSlotTable;
extern char *g_PmSlotTable2;

int Overlay_LoadTables(void) {
    int scratch;
    int status;
    int imageBase;
    int *state;
    u16 *range;
    int table;
    int main_table;
    int large_table;
    int i;

    imageBase = g_PeImageBaseLba;

    VSync(0);
    SetDispMask(0);
    state = g_GameState;
    CD_ReadSectors(1, 0xCC, 0, g_SceneLoadScratchBuffer, 0x21, 1);

retry_first_load:
    range = g_MainAssetLbaTbl;
    do {
        status = CdRom_ReadSectors(imageBase + range[0], 0, state[0x57], range[1] - range[0]);
    } while (status == -1);
    while (1) {
        status = Sys_VSyncTimeout((int)&scratch);
        {
            int poll;
            poll = status + 1;
            if ((unsigned int)poll < 2U) {
                poll = status;
                D_800B0CD8_word &= 0xFEFFBFFF;
                asm volatile("" : "=r"(status) : "0"(status));
                poll = status;
            } else {
                poll = status;
            }
            if (poll == 0) {
                break;
            }
            if (poll == -1) {
                goto retry_first_load;
            }
        }
    }

    main_table = state[0x57];
    for (i = 0; i < 3; i++) {
        table = ((short)i << 2) + main_table;
        Gpu_LoadTimImage(main_table + *(int *)table);
    }

    if (D_800A77FC & 0x2000) {
retry_large_load_a:
        range = g_LargeTexLbaTbl;
        do {
            status = CdRom_ReadSectors(imageBase + range[0], 0, state[0x5B], range[1] - range[0]);
        } while (status == -1);
        while (1) {
            status = Sys_VSyncTimeout((int)&scratch);
            {
                int poll;
                poll = status + 1;
                if ((unsigned int)poll < 2U) {
                    poll = status;
                    D_800B0CD8_word &= 0xFEFFBFFF;
                    asm volatile("" : "=r"(status) : "0"(status));
                    poll = status;
                } else {
                    poll = status;
                }
                if (poll == 0) {
                    goto large_load_done;
                }
                if (poll == -1) {
                    goto retry_large_load_a;
                }
            }
        }
    } else {
retry_large_load_b:
        range = D_8009316C;
        do {
            status = CdRom_ReadSectors(imageBase + range[0], 0, state[0x5B], range[1] - range[0]);
        } while (status == -1);
        while (1) {
            status = Sys_VSyncTimeout((int)&scratch);
            {
                int poll;
                poll = status + 1;
                if ((unsigned int)poll < 2U) {
                    poll = status;
                    D_800B0CD8_word &= 0xFEFFBFFF;
                    asm volatile("" : "=r"(status) : "0"(status));
                    poll = status;
                } else {
                    poll = status;
                }
                if (poll == 0) {
                    break;
                }
                if (poll == -1) {
                    goto retry_large_load_b;
                }
            }
        }
    }

large_load_done:
    large_table = state[0x5B];
    for (i = 0; i < 0x106; i++) {
        table = ((short)i << 2) + large_table;
        Gpu_LoadTimImage(large_table + *(int *)table);
    }

    DrawSync(0);

retry_final_load:
    range = g_FontUiLbaTbl;
    do {
        status = CdRom_ReadSectors(g_PeImageBaseLba + range[0], 0, g_StrFileDirBuffer, range[1] - range[0]);
    } while (status == -1);
    while (1) {
        status = Sys_VSyncTimeout((int)&scratch);
        {
            register int poll asm("$2");
            poll = status + 1;
            if ((unsigned int)poll < 2U) {
                poll = status;
                D_800B0CD8_word &= 0xFEFFBFFF;
                asm volatile("" : "=r"(status) : "0"(status));
                poll = status;
            } else {
                poll = status;
            }
            if (poll == 0) {
                break;
            }
            if (poll == -1) {
                goto retry_final_load;
            }
        }
    }

    EnterCriticalSection();
    FlushCache();
    ExitCriticalSection();
    return 0;
}

INCLUDE_ASM("asm/USA/main/nonmatchings/main/misc19", CD_LoadBootAudio);

int Pm_AllocSlot(unsigned int cmd) {
    int result;
    int i;
    char *entry;

    result = -1;
    if (cmd >= 0xC0) {
        return -1;
    }

    if ((unsigned int)(cmd - 0x46) < 0xF) {
        i = 0;
        entry = g_PmSlotTable2;
        do {
            if (*(u8 *)entry == 0) {
                result = i + 0xB;
                goto done;
            }
            i++;
            entry += 0x10C;
        } while (i < 0xB);
        return result;
    }

    i = 0;
    entry = g_PmSlotTable;
    do {
        if (*(u8 *)entry == 0) {
            result = i;
            goto done;
        }
        i++;
        entry += 0xA0C;
    } while (i < 0xB);

done:
    return result;
}
