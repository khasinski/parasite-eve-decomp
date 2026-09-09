#include "pe1/psyq_bios.h"

PSYQ_BIOS_TRAMPOLINE(strcpy, 0xA0, 0x19);

static unsigned int tail_alignment_62220[] __attribute__((section(".text"))) = { 0x00000000 };
