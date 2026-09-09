#include "pe1/psyq_bios.h"

PSYQ_BIOS_TRAMPOLINE(StopCARD2, 0xB0, 0x4C);

static unsigned int tail_alignment_6EB40[] __attribute__((section(".text"))) = { 0x00000000 };
