#include "pe1/psyq_bios.h"

PSYQ_BIOS_TRAMPOLINE(bzero, 0xA0, 0x28);

static unsigned int tail_alignment_62230[] __attribute__((section(".text"))) = { 0x00000000 };
