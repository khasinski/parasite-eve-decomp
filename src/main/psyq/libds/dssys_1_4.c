/* ASSEMBLER: GNU */
/* Psy-Q LIBDS DSSYS_1.OBJ, part 4 of 11: DS_cw, DS_vsync_callback, DS_ready_callback, DS_start_callback, DS_sync_callback, DS_system_status, DS_lastcom, DS_lastmode, DS_lastpos, DS_lastread, DS_lastseek, DS_status, DS_sync, DS_ready, DS_shell_open, DS_cw_system. */
#include "pe1/psyq_cd.h"
#include "pe1/psyq_ds.h"
#include "pe1/cdrom.h"

s32 LIBDS_DSSYS_1_text_368(s32 arg0);

s32 DS_cw(s32 arg0) {
    register int *base asm("$6");
    register int *prev asm("$7");

    base = &g_CdRomCmdTimeout;
    asm volatile("" : "=r"(base) : "0"(base));
    if (base[0] > 0) {
        return 0;
    }
    if (base[-0x11] == 0) {
        prev = base - 0x11;
        return 0;
    }
    prev = base - 0x11;
    asm volatile("" : "=r"(prev) : "0"(prev));
    if (base[-9] != 1) {
        return 0;
    }
    prev[7] = 0x1F;
    base[-9] = 2;
    return LIBDS_DSSYS_1_text_368((prev[9] = 0xB, arg0 & 0xFF));
}

extern unsigned int D_800A36A0;
extern unsigned int D_800A36A4;
extern unsigned int D_800A36A8;
extern unsigned int D_800A36AC;

void DS_vsync_callback(unsigned int value) {
    D_800A36A0 = value;
}

void DS_sync_callback(unsigned int value) {
    D_800A36A4 = value;
}

void DS_ready_callback(unsigned int value) {
    D_800A36A8 = value;
}

void DS_start_callback(unsigned int value) {
    D_800A36AC = value;
}

u32 DS_system_status(u32 mode) {
    u32 offset;
    u32 table_page;

    offset = mode << 2;
    /* g_DsReadStatusBlock is at 0x8009B574 in the USA image. */
    table_page = 0x800A0000u;
    return *(u32 *)(table_page + offset - 0x4A8Cu);
}

extern unsigned char g_CdLastCmd;

int DS_lastcom(void) {
    return g_CdLastCmd;
}

extern unsigned char g_CdCmdMode;

int DS_lastmode(void) {
    return g_CdCmdMode;
}

CdlLOC *DS_lastpos(void) {
    return &g_CdCurPosPtr;
}

extern unsigned char g_CdRetryCount;

int DS_lastseek(void) {
    return g_CdRetryCount;
}

extern unsigned char g_CdCmdParam;

int DS_lastread(void) {
    return g_CdCmdParam;
}

int DS_status(void) {
    return g_CdSeekState.eventStatus;
}

void DS_sync(u8 *result) {
    CD_sync(1, result);
}

void DS_ready(u8 *result) {
    CD_ready(1, result);
}

extern int g_CdDiscType;

int DS_shell_open(void) {
    return g_CdDiscType;
}

int LIBDS_DSSYS_1_text_368(int arg0);

int DS_cw_system(int arg0) {
    volatile int *ptr;
    ptr = &g_CdRomCmdTimeout;
    if (*ptr > 0) {
        return 0;
    }
    asm volatile("" : "=r"(ptr) : "0"(ptr));
    ptr[-10] = 0x20;
    return LIBDS_DSSYS_1_text_368(arg0 & 0xFF);
}
