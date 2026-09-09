#include "pe1/psyq_bios.h"

PSYQ_BIOS_TRAMPOLINE(SysDeqIntRP, 0xC0, 0x3);

static unsigned int tail_alignment_6EA00[] __attribute__((section(".text"))) = { 0x00000000 };
