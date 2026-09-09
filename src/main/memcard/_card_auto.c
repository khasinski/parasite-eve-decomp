#include "pe1/psyq_bios.h"

PSYQ_BIOS_TRAMPOLINE(_card_auto, 0xA0, 0xAD);

static unsigned int tail_alignment_6E570[] __attribute__((section(".text"))) = { 0x00000000 };
