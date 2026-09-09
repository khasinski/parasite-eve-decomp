#include "pe1/psyq_bios.h"

PSYQ_BIOS_TRAMPOLINE(open, 0xB0, 0x32);

static unsigned int open_alignment[] __attribute__((section(".text"))) = { 0x00000000 };
