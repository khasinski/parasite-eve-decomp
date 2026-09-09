#include "pe1/psyq_bios.h"

PSYQ_BIOS_TRAMPOLINE(_card_load, 0xA0, 0xAC);

static unsigned int tail_alignment_6E560[] __attribute__((section(".text"))) = { 0x00000000 };
