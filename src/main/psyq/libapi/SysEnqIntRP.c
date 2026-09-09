#include "pe1/psyq_bios.h"

PSYQ_BIOS_TRAMPOLINE(SysEnqIntRP, 0xC0, 0x2);

static unsigned int tail_alignment_6E9F0[] __attribute__((section(".text"))) = { 0x00000000 };
