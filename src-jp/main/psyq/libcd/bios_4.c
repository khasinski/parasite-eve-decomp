/* ASSEMBLER: GNU */
/* CC1_FLAGS: -fno-schedule-insns -fno-schedule-insns2 */
/* Psy-Q LIBCD BIOS.OBJ, part 4 of 7: CD_initvol, CD_initintr. */

#include "common.h"
#include "pe1/psyq_callbacks.h"
#include "pe1/psyq_cd.h"

typedef struct CdInitVolFrame {
    u8 packet[4];
    u32 pad04;
} CdInitVolFrame;

register CdInitVolFrame *g_CdInitVolFrame asm("$29");
register int g_CdInitVolValue asm("$2");

/* BIOS_1.OBJ state. Nothing in this executable references
 * g_CdReadCallback. */
CdlCB g_CdSyncCallback = 0;
CdlCB g_CdReadyCallback = 0;
CdlCB g_CdReadCallback = 0;
int CD_debug = 0;
u32 g_CdStatus = 0;
u32 g_CdStatus1 = 0;
int g_CdLidOpenCount = 0;
CdlLOC g_CdLastPos = {2, 0, 0, 0};
u8 g_CdMode = 0;
u8 g_CdLastCom = 0;
u32 g_CdDsActive = 0;

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

u32 g_CdCommandAckOnly[32] = {
    0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0,
    0, 0, 1, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0,
};

CdCommandTables g_CdCommandTables = {
    {
        0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0,
    },
    {
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    },
    {
        0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 0,
        0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    },
};

/* Hardware register pointers: the CD controller's four ports, the common
 * delay register (written 0x1325) and the SPU base; after the interrupt
 * event bytes and the init descriptor, the CD bus delay/size register, the
 * DMA control register and the channel 3 (CD) address register. */
volatile u8 *g_CdRegIndexBase = (volatile u8 *)0x1F801800;
volatile u8 *g_CdRegPort1 = (volatile u8 *)0x1F801801;
volatile u8 *g_CdRegDataWrite = (volatile u8 *)0x1F801802;
volatile u8 *g_CdRegResponse = (volatile u8 *)0x1F801803;
volatile u32 *g_CdRegRequest = (volatile u32 *)0x1F801020;
u16 *volatile g_CdSpuRegBase = (u16 *)0x1F801C00;
CdInterruptEvents g_CdInterruptEvents = {0, 0, 0};

CdBiosInitDescriptor g_CdBiosInitDescriptor = {
    &g_CdInterruptEvents,
    g_CdSyncResult,
    &g_CdLastCom,
    &g_CdStatus,
    &g_CdLastPos,
    g_CdBiosVersion,
};

volatile u32 *g_CdRegDelaySize = (volatile u32 *)0x1F801018;
volatile u32 *g_CdRegDmaPcr = (volatile u32 *)0x1F8010F0;
void *volatile *g_CdRegDmaMadr = (void *volatile *)0x1F8010B0;

int CD_initvol(void) {
    volatile void *io;
    io = g_CdSpuRegBase;
    g_CdInitVolValue = ((u16 *)io)[0x1B8 / 2];
    g_CdInitVolFrame--;
    if (g_CdInitVolValue == 0) {
        g_CdInitVolValue = ((u16 *)io)[0x1BA / 2];
        if (g_CdInitVolValue == 0) {
            g_CdInitVolValue = 0x3FFF;
            ((u16 *)io)[0x180 / 2] = g_CdInitVolValue;
            ((u16 *)io)[0x182 / 2] = g_CdInitVolValue;
            io = g_CdSpuRegBase;
        }
    }
    g_CdInitVolValue = 0x3FFF;
    ((u16 *)io)[0x1B0 / 2] = g_CdInitVolValue;
    ((u16 *)io)[0x1B2 / 2] = g_CdInitVolValue;
    g_CdInitVolValue = 0xC001;
    ((u16 *)io)[0x1AA / 2] = g_CdInitVolValue;

    io = g_CdRegIndexBase;
    g_CdInitVolValue = 0x80;
    g_CdInitVolFrame->packet[2] = g_CdInitVolValue;
    g_CdInitVolFrame->packet[0] = g_CdInitVolValue;
    g_CdInitVolValue = 2;
    g_CdInitVolFrame->packet[3] = 0;
    g_CdInitVolFrame->packet[1] = 0;
    *(volatile u8 *)io = g_CdInitVolValue;

    io = g_CdRegDataWrite;
    g_CdInitVolValue = g_CdInitVolFrame->packet[0];
    *(volatile u8 *)io = g_CdInitVolValue;
    io = g_CdRegResponse;
    g_CdInitVolValue = g_CdInitVolFrame->packet[1];
    *(volatile u8 *)io = g_CdInitVolValue;
    io = g_CdRegIndexBase;
    g_CdInitVolValue = 3;
    *(volatile u8 *)io = g_CdInitVolValue;
    io = g_CdRegPort1;
    g_CdInitVolValue = g_CdInitVolFrame->packet[2];
    *(volatile u8 *)io = g_CdInitVolValue;
    io = g_CdRegDataWrite;
    g_CdInitVolValue = g_CdInitVolFrame->packet[3];
    *(volatile u8 *)io = g_CdInitVolValue;
    io = g_CdRegResponse;
    g_CdInitVolValue = 0x20;
    *(volatile u8 *)io = g_CdInitVolValue;

    g_CdInitVolValue = 0;
    g_CdInitVolFrame++;
    return g_CdInitVolValue;
}

void CD_initintr(void) {
    g_CdReadyCallback = 0;
    g_CdSyncCallback = 0;
    g_CdStatus1 = 0;
    g_CdStatus = 0;
    ResetCallback();
    InterruptCallback(2, callback);
}
