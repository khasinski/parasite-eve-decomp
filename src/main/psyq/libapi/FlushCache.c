#include "pe1/psyq_bios.h"

PSYQ_BIOS_TRAMPOLINE(FlushCache, 0xA0, 0x44);

static unsigned int FlushCache_alignment[] __attribute__((section(".text"))) = { 0x00000000 };
