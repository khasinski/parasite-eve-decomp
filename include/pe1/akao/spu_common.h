#ifndef PE1_AKAO_SPU_COMMON_H
#define PE1_AKAO_SPU_COMMON_H

#include "pe1/psyq_spu_api.h"

/* Common attributes occupy the first 40 bytes of this shared AKAO area. */
extern unsigned char D_800C0D90[];

/* Independently addressed aliases of common.cd.volume and common.cd.reverb. */
extern short D_800C0DA0, D_800C0DA2;
extern int D_800C0DA4;

#endif
