#include "pe1/psyq_bios.h"

PSYQ_BIOS_TRAMPOLINE(exit, 0xB0, 0x38);

static unsigned int tail_alignment_621F0[] __attribute__((section(".text"))) = { 0x00000000 };
