#include "pe1/psyq_bios.h"

PSYQ_BIOS_TRAMPOLINE(_bu_init, 0xA0, 0x70);

static unsigned int _bu_init_alignment[] __attribute__((section(".text"))) = { 0x00000000 };
