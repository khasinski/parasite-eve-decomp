#include "pe1/psyq_bios.h"

PSYQ_BIOS_TRAMPOLINE(printf, 0xA0, 0x3F);

static unsigned int tail_alignment_62280[] __attribute__((section(".text"))) = { 0x00000000 };
