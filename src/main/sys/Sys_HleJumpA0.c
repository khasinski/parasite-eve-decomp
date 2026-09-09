#include "pe1/psyq_bios.h"

PSYQ_BIOS_TRAMPOLINE(Sys_HleJumpA0, 0xA0, 0x72);

static unsigned int tail_alignment_64B78[] __attribute__((section(".text"))) = { 0x00000000, 0x00000000, 0x00000000 };
