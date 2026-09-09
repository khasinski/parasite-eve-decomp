#include "pe1/psyq_bios.h"

PSYQ_BIOS_TRAMPOLINE(GPU_cw, 0xA0, 0x49);

static unsigned int GPU_cw_alignment[] __attribute__((section(".text"))) = { 0x00000000 };
