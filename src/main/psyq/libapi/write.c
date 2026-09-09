#include "pe1/psyq_bios.h"

PSYQ_BIOS_TRAMPOLINE(write, 0xB0, 0x35);

static unsigned int write_alignment[] __attribute__((section(".text"))) = { 0x00000000 };
