#include "pe1/psyq_cd.h"

typedef struct CdCurrentPosPage {
    char reserved00[0x4A7E];
} CdCurrentPosPage;

register CdCurrentPosPage *g_CdCurrentPosPage asm("$2");

CdlLOC *CdRom_GetCurrentPosPtr(void) {
    g_CdCurrentPosPage = (CdCurrentPosPage *)0x800A0000;
    return (CdlLOC *)&g_CdCurrentPosPage[-1];
}
