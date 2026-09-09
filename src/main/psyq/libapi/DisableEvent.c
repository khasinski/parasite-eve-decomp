#include "pe1/psyq_bios.h"

PSYQ_BIOS_TRAMPOLINE(DisableEvent, 0xB0, 0xD);

static unsigned int tail_alignment_76000[] __attribute__((section(".text"))) = { 0x00000000 };
