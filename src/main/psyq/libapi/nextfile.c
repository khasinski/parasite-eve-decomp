#include "pe1/psyq_bios.h"

PSYQ_BIOS_TRAMPOLINE(nextfile, 0xB0, 0x43);

static unsigned int nextfile_alignment[] __attribute__((section(".text"))) = { 0x00000000 };
