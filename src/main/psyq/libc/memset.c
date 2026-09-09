#include "pe1/psyq_bios.h"

PSYQ_BIOS_TRAMPOLINE(memset, 0xA0, 0x2B);

static unsigned int tail_alignment_62250[] __attribute__((section(".text"))) = { 0x00000000 };
