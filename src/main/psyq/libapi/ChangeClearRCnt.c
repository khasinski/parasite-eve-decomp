#include "pe1/psyq_bios.h"

PSYQ_BIOS_TRAMPOLINE(ChangeClearRCnt, 0xC0, 0xA);

static unsigned int tail_alignment_64490[] __attribute__((section(".text"))) = { 0x00000000 };
