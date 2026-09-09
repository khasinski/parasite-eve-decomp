#include "pe1/psyq_bios.h"

PSYQ_BIOS_TRAMPOLINE(StartCARD2, 0xB0, 0x4B);

static unsigned int tail_alignment_6EB30[] __attribute__((section(".text"))) = { 0x00000000 };
