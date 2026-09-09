#include "pe1/psyq_bios.h"

PSYQ_BIOS_TRAMPOLINE(erase, 0xB0, 0x45);

static unsigned int erase_alignment[] __attribute__((section(".text"))) = { 0x00000000 };
