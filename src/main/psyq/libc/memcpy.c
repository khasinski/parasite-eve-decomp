#include "pe1/psyq_bios.h"

PSYQ_BIOS_TRAMPOLINE(memcpy, 0xA0, 0x2A);

static unsigned int tail_alignment_62240[] __attribute__((section(".text"))) = { 0x00000000 };
