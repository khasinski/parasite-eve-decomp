#include "pe1/psyq_bios.h"

PSYQ_BIOS_TRAMPOLINE(InitHeap, 0xA0, 0x39);

static unsigned int InitHeap_alignment[] __attribute__((section(".text"))) = { 0x00000000 };
