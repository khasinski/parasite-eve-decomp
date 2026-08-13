#include "pe1/psyq_cd.h"

typedef struct CdLastPositionPage {
    unsigned char padding[0xAFD0];
    CdlLOC position;
    unsigned char tail[0x10000 - 0xAFD0 - sizeof(CdlLOC)];
} CdLastPositionPage;

register CdLastPositionPage * volatile g_CdLastPositionPage asm("$2");

CdlLOC *CdLastPos(void) {
    g_CdLastPositionPage = (CdLastPositionPage *)0x800A0000;
    return &g_CdLastPositionPage[-1].position;
}
