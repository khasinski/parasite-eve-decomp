#include "pe1/psyq_bios.h"

PSYQ_BIOS_TRAMPOLINE(memchr, 0xA0, 0x2E);

static unsigned int tail_alignment_62B30[] __attribute__((section(".text"))) = { 0x00000000 };
