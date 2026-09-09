#include "pe1/psyq_bios.h"

PSYQ_BIOS_TRAMPOLINE(strlen, 0xA0, 0x1B);

static unsigned int tail_alignment_62B20[] __attribute__((section(".text"))) = { 0x00000000 };
