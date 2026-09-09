#include "pe1/psyq_bios.h"

PSYQ_BIOS_TRAMPOLINE(_card_info, 0xA0, 0xAB);

static unsigned int tail_alignment_6E550[] __attribute__((section(".text"))) = { 0x00000000 };
