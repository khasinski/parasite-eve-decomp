#include "pe1/psyq_bios.h"

PSYQ_BIOS_TRAMPOLINE(lseek, 0xB0, 0x33);

static unsigned int lseek_alignment[] __attribute__((section(".text"))) = { 0x00000000 };
