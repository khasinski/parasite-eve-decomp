#include "pe1/psyq_bios.h"

PSYQ_BIOS_TRAMPOLINE(WaitEvent, 0xB0, 0xA);

static unsigned int tail_alignment_7DFBC[] __attribute__((section(".text"))) = { 0x00000000 };
