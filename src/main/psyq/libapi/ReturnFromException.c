#include "pe1/psyq_bios.h"

PSYQ_BIOS_TRAMPOLINE(ReturnFromException, 0xB0, 0x17);

static unsigned int tail_alignment_64B90[] __attribute__((section(".text"))) = { 0x00000000 };
