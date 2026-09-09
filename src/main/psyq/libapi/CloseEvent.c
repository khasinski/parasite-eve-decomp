#include "pe1/psyq_bios.h"

PSYQ_BIOS_TRAMPOLINE(CloseEvent, 0xB0, 0x9);

static unsigned int tail_alignment_75FF0[] __attribute__((section(".text"))) = { 0x00000000 };
