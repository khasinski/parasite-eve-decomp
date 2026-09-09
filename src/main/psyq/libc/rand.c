#include "pe1/psyq_bios.h"

PSYQ_BIOS_TRAMPOLINE(rand, 0xA0, 0x2F);

static unsigned int tail_alignment_62260[] __attribute__((section(".text"))) = { 0x00000000 };
