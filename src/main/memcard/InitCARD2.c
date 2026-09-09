#include "pe1/psyq_bios.h"

PSYQ_BIOS_TRAMPOLINE(InitCARD2, 0xB0, 0x4A);

static unsigned int tail_alignment_6EB20[] __attribute__((section(".text"))) = { 0x00000000 };
