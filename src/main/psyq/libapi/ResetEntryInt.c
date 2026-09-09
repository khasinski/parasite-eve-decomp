#include "pe1/psyq_bios.h"

PSYQ_BIOS_TRAMPOLINE(ResetEntryInt, 0xB0, 0x18);

static unsigned int tail_alignment_64BA0[] __attribute__((section(".text"))) = { 0x00000000 };
