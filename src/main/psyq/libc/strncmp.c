#include "pe1/psyq_bios.h"

PSYQ_BIOS_TRAMPOLINE(strncmp, 0xA0, 0x18);

static unsigned int tail_alignment_62210[] __attribute__((section(".text"))) = { 0x00000000 };
