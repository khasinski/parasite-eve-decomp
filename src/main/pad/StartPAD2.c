#include "pe1/psyq_bios.h"

PSYQ_BIOS_TRAMPOLINE(StartPAD2, 0xB0, 0x13);

static unsigned int tail_alignment_6E9C0[] __attribute__((section(".text"))) = { 0x00000000 };
