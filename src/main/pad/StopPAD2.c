#include "pe1/psyq_bios.h"

PSYQ_BIOS_TRAMPOLINE(StopPAD2, 0xB0, 0x14);

static unsigned int tail_alignment_6E9D0[] __attribute__((section(".text"))) = { 0x00000000 };
