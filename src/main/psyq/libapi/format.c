#include "pe1/psyq_bios.h"

PSYQ_BIOS_TRAMPOLINE(format, 0xB0, 0x41);

static unsigned int format_alignment[] __attribute__((section(".text"))) = { 0x00000000 };
