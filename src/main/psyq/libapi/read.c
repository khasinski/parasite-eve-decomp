#include "pe1/psyq_bios.h"

PSYQ_BIOS_TRAMPOLINE(read, 0xB0, 0x34);

static unsigned int read_alignment[] __attribute__((section(".text"))) = { 0x00000000 };
