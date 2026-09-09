#include "pe1/psyq_bios.h"

PSYQ_BIOS_TRAMPOLINE(HookEntryInt, 0xB0, 0x19);

static unsigned int tail_alignment_64BB0[] __attribute__((section(".text"))) = { 0x00000000 };
