#include "pe1/psyq_bios.h"

PSYQ_BIOS_TRAMPOLINE(firstfile, 0xB0, 0x42);

static unsigned int firstfile_alignment[] __attribute__((section(".text"))) = { 0x00000000 };
