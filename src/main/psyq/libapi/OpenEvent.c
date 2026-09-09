#include "pe1/psyq_bios.h"

PSYQ_BIOS_TRAMPOLINE(OpenEvent, 0xB0, 0x8);

static unsigned int OpenEvent_alignment[] __attribute__((section(".text"))) = { 0x00000000 };
