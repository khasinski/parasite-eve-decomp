#include "pe1/psyq_bios.h"

PSYQ_BIOS_TRAMPOLINE(srand, 0xA0, 0x30);

static unsigned int tail_alignment_62270[] __attribute__((section(".text"))) = { 0x00000000 };
