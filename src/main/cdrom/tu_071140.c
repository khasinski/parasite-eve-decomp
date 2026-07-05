#include "include_asm.h"

extern int g_DsReadSysEnabled;

int CdRom_IsDsReadSystemEnabled(void);

extern void CD_vol(void);

extern int CD_getsector2(void);

extern int CD_getsector(void);

extern int CD_datasync(void);

int CdRom_DataSync(void);

typedef unsigned char u_char;

typedef struct {
    u_char minute;
    u_char second;
    u_char sector;
    u_char track;
} CdlLOC;

int CdRom_IsDsReadSystemEnabled(void) {
    return g_DsReadSysEnabled;
}

void Util_Copy4(unsigned char *dst, unsigned char *src) {
    int i;

    if (src) {
        if (dst) {
            for (i = 0; i < 4; i++) {
                *dst++ = *src++;
            }
        }
    } else if (dst) {
        *dst = 0;
    }
}

void Util_Copy8(unsigned char *dst, unsigned char *src) {
    int i;

    if (src) {
        if (dst) {
            for (i = 0; i < 8; i++) {
                *dst++ = *src++;
            }
        }
    } else if (dst) {
        *dst = 0;
    }
}

INCLUDE_ASM("asm/USA/main/nonmatchings/cdrom/tu_071140", DS_status);

int Akao_SetCdMixVolume(void) {
    CD_vol();
    return 1;
}

int CdRom_IsBusy(void) {
    return CD_getsector2() == 0;
}

int CdRom_IsBusy2(void) {
    return CD_getsector() == 0;
}

int CdRom_DataSync(void) {
    return CD_datasync();
}

static inline int ENCODE_BCD(int n) { return ((n / 10) << 4) + (n % 10); }

CdlLOC *CdIntToPos(int i, CdlLOC *p) {
    i += 150;
    p->sector = ENCODE_BCD(i % 75);
    p->second = ENCODE_BCD(i / 75 % 60);
    p->minute = ENCODE_BCD(i / 75 / 60);
    return p;
}

int CdPosToInt(CdlLOC *p) {
#define DECODE_BCD(x) (((x) >> 4) * 10 + ((x) & 0xF))
    u_char sector = p->sector;
    u_char second = p->second;
    u_char minute = p->minute;

    return (DECODE_BCD(minute) * 60 + DECODE_BCD(second)) * 75 +
           DECODE_BCD(sector) - 150;
}
