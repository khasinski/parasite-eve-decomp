#include "pe1/psyq_bios.h"

PSYQ_BIOS_TRAMPOLINE(strcat, 0xA0, 0x15);

static unsigned int tail_alignment_62200[] __attribute__((section(".text"))) = { 0x00000000 };
