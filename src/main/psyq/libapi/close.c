#include "pe1/psyq_bios.h"

PSYQ_BIOS_TRAMPOLINE(close, 0xB0, 0x36);

static unsigned int close_alignment[] __attribute__((section(".text"))) = { 0x00000000 };
