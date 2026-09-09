#include "pe1/psyq_bios.h"

PSYQ_BIOS_TRAMPOLINE(_card_write, 0xB0, 0x4E);

static unsigned int tail_alignment_6E5C0[] __attribute__((section(".text"))) = { 0x00000000 };
