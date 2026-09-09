#include "pe1/psyq_bios.h"

PSYQ_BIOS_TRAMPOLINE(EnableEvent, 0xB0, 0xC);

static unsigned int EnableEvent_alignment[] __attribute__((section(".text"))) = { 0x00000000 };
