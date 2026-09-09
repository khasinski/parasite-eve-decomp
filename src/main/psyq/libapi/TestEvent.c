#include "pe1/psyq_bios.h"

PSYQ_BIOS_TRAMPOLINE(TestEvent, 0xB0, 0xB);

static unsigned int TestEvent_alignment[] __attribute__((section(".text"))) = { 0x00000000 };
