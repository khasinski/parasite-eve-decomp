#include "pe1/psyq_bios.h"

PSYQ_BIOS_TRAMPOLINE(InitPAD2, 0xB0, 0x12);

static unsigned int tail_alignment_6E9B0[] __attribute__((section(".text"))) = { 0x00000000 };
