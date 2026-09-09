#include "pe1/psyq_bios.h"

PSYQ_BIOS_TRAMPOLINE(_new_card, 0xB0, 0x50);

static unsigned int tail_alignment_6E5D0[] __attribute__((section(".text"))) = { 0x00000000 };
