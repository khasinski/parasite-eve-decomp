#include "pe1/psyq_bios.h"

PSYQ_BIOS_TRAMPOLINE(UnDeliverEvent, 0xB0, 0x20);

static unsigned int tail_alignment_76010[] __attribute__((section(".text"))) = { 0x00000000 };
