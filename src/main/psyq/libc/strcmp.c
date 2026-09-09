#include "pe1/psyq_bios.h"

PSYQ_BIOS_TRAMPOLINE(strcmp, 0xA0, 0x17);

static unsigned int tail_alignment_63260[] __attribute__((section(".text"))) = { 0x00000000 };
