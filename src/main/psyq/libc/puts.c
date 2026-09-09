#include "pe1/psyq_bios.h"

PSYQ_BIOS_TRAMPOLINE(puts, 0xB0, 0x3F);

static unsigned int tail_alignment_64468[] __attribute__((section(".text"))) = { 0x00000000, 0x00000000, 0x00000000 };
