#include "pe1/psyq_callbacks.h"
#include "common.h"
#include "pe1/psyq_cd.h"
/* CC1_FLAGS: -fno-schedule-insns -fno-schedule-insns2 */

extern u16 *volatile D_8009B290;
extern void Cd_SetIntrMask(void);

typedef struct CdInitVolFrame {
    u8 packet[4];
    u32 pad04;
} CdInitVolFrame;

register CdInitVolFrame *g_CdInitVolFrame asm("$29");
register int g_CdInitVolValue asm("$2");
register volatile void *g_CdInitVolIo asm("$3");
register CdCallbackDataPage *g_CdInitCallbackPage asm("$1");

/* BIOS_1.OBJ state. Parasite Eve adds cd_read_callback between cd_debug and
 * cd_status, shifting the remainder of the SDK layout by one word. */
typedef struct CdBiosState {
    CdlCB sync_callback;
    CdlCB ready_callback;
    s32 debug;
    CdlCB read_callback;
    u32 status;
    u32 status1;
    u32 lid_open_count;
    u8 position[4];
    u8 mode;
    u8 command;
    u8 pad26[2];
    u32 ds_active;
} CdBiosState;

typedef char CdBiosState_size[(sizeof(CdBiosState) == 0x28) ? 1 : -1];

CdBiosState g_CdBiosState __attribute__((section(".data"))) = {
    0, 0, 0, 0, 0, 0, 0, {2, 0, 0, 0}, 0, 0, {0, 0}, 0,
};

typedef struct CdBiosNames {
    char read_s[12];
    char seek_p[12];
    char seek_l[12];
    char get_td[12];
    char get_tn[12];
    char get_loc_p[12];
    char get_loc_l[12];
    char unknown[4];
    char set_mode[12];
    char set_filter[16];
    char demute[12];
    char mute[8];
    char reset[12];
    char pause[12];
    char stop[8];
    char standby[12];
    char read_n[12];
    char backward[12];
    char forward[12];
    char play[8];
    char set_loc[12];
    char nop[8];
    char sync[8];
    char disk_error[12];
    char data_end[8];
    char acknowledge[12];
    char complete[12];
    char data_ready[12];
    char no_intr[8];
} CdBiosNames;

typedef char CdBiosNames_size[(sizeof(CdBiosNames) == 0x13C) ? 1 : -1];

const CdBiosNames g_CdBiosNames = {
    "CdlReadS", "CdlSeekP", "CdlSeekL", "CdlGetTD", "CdlGetTN",
    "CdlGetlocP", "CdlGetlocL", "?", "CdlSetmode", "CdlSetfilter",
    "CdlDemute", "CdlMute", "CdlReset", "CdlPause", "CdlStop",
    "CdlStandby", "CdlReadN", "CdlBackward", "CdlForward", "CdlPlay",
    "CdlSetloc", "CdlNop", "CdlSync", "DiskError", "DataEnd",
    "Acknowledge", "Complete", "DataReady", "NoIntr",
};

const char *g_CdCommandNames[32] = {
    g_CdBiosNames.sync, g_CdBiosNames.nop, g_CdBiosNames.set_loc,
    g_CdBiosNames.play, g_CdBiosNames.forward, g_CdBiosNames.backward,
    g_CdBiosNames.read_n, g_CdBiosNames.standby, g_CdBiosNames.stop,
    g_CdBiosNames.pause, g_CdBiosNames.reset, g_CdBiosNames.mute,
    g_CdBiosNames.demute, g_CdBiosNames.set_filter, g_CdBiosNames.set_mode,
    g_CdBiosNames.unknown, g_CdBiosNames.get_loc_l, g_CdBiosNames.get_loc_p,
    g_CdBiosNames.unknown, g_CdBiosNames.get_tn, g_CdBiosNames.get_td,
    g_CdBiosNames.seek_l, g_CdBiosNames.seek_p, g_CdBiosNames.unknown,
    g_CdBiosNames.unknown, g_CdBiosNames.unknown, g_CdBiosNames.unknown,
    g_CdBiosNames.read_s, g_CdBiosNames.unknown, g_CdBiosNames.unknown,
    g_CdBiosNames.unknown, g_CdBiosNames.unknown,
};

const char *g_CdInterruptNames[8] = {
    g_CdBiosNames.no_intr, g_CdBiosNames.data_ready, g_CdBiosNames.complete,
    g_CdBiosNames.acknowledge, g_CdBiosNames.data_end,
    g_CdBiosNames.disk_error, g_CdBiosNames.unknown, g_CdBiosNames.unknown,
};

int CD_initvol(void) {
    g_CdInitVolIo = D_8009B290;
    g_CdInitVolValue = ((u16 *)g_CdInitVolIo)[0x1B8 / 2];
    g_CdInitVolFrame--;
    if (g_CdInitVolValue != 0) {
        goto set_default;
    }
    g_CdInitVolValue = ((u16 *)g_CdInitVolIo)[0x1BA / 2];
    if (g_CdInitVolValue != 0) {
        g_CdInitVolValue = 0x3FFF;
        goto store_common;
    }
    g_CdInitVolValue = 0x3FFF;
    ((u16 *)g_CdInitVolIo)[0x180 / 2] = g_CdInitVolValue;
    ((u16 *)g_CdInitVolIo)[0x182 / 2] = g_CdInitVolValue;
    g_CdInitVolIo = D_8009B290;

set_default:
    g_CdInitVolValue = 0x3FFF;

store_common:
    ((u16 *)g_CdInitVolIo)[0x1B0 / 2] = g_CdInitVolValue;
    ((u16 *)g_CdInitVolIo)[0x1B2 / 2] = g_CdInitVolValue;
    g_CdInitVolValue = 0xC001;
    ((u16 *)g_CdInitVolIo)[0x1AA / 2] = g_CdInitVolValue;

    g_CdInitVolIo = g_CdRegIndexBase;
    g_CdInitVolValue = 0x80;
    g_CdInitVolFrame->packet[2] = g_CdInitVolValue;
    g_CdInitVolFrame->packet[0] = g_CdInitVolValue;
    g_CdInitVolValue = 2;
    g_CdInitVolFrame->packet[3] = 0;
    g_CdInitVolFrame->packet[1] = 0;
    *(volatile u8 *)g_CdInitVolIo = g_CdInitVolValue;

    g_CdInitVolIo = g_CdRegDataWrite;
    g_CdInitVolValue = g_CdInitVolFrame->packet[0];
    *(volatile u8 *)g_CdInitVolIo = g_CdInitVolValue;
    g_CdInitVolIo = g_CdRegResponse;
    g_CdInitVolValue = g_CdInitVolFrame->packet[1];
    *(volatile u8 *)g_CdInitVolIo = g_CdInitVolValue;
    g_CdInitVolIo = g_CdRegIndexBase;
    g_CdInitVolValue = 3;
    *(volatile u8 *)g_CdInitVolIo = g_CdInitVolValue;
    g_CdInitVolIo = g_CdRegPort1;
    g_CdInitVolValue = g_CdInitVolFrame->packet[2];
    *(volatile u8 *)g_CdInitVolIo = g_CdInitVolValue;
    g_CdInitVolIo = g_CdRegDataWrite;
    g_CdInitVolValue = g_CdInitVolFrame->packet[3];
    *(volatile u8 *)g_CdInitVolIo = g_CdInitVolValue;
    g_CdInitVolIo = g_CdRegResponse;
    g_CdInitVolValue = 0x20;
    *(volatile u8 *)g_CdInitVolIo = g_CdInitVolValue;

    g_CdInitVolValue = 0;
    g_CdInitVolFrame++;
    return g_CdInitVolValue;
}

void CD_initintr(void) {
    g_CdReadyCallback = 0;
    g_CdSyncCallback = 0;
    D_8009AFC8 = 0;
    g_CdInitCallbackPage = (CdCallbackDataPage *)0x800A0000;
    g_CdInitCallbackPage[-1].status = 0;
    ResetCallback();
    InterruptCallback(2, Cd_SetIntrMask);
}
