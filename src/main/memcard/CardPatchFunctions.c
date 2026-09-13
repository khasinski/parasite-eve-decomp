#include "pe1/psyq_card.h"

/* The copied BIOS patch enters with its I/O base in v1, not in a0. */
register CardPatchIoRegisters *cardPatchIo asm("$3");
void CardPatchFunctions(void) {
    if (cardPatchIo->interruptMask & 0x80) {
        while (cardPatchIo->serialStatus & 0x80) {
        }
        /* GNU computed goto: continue the installed BIOS code without
         * changing ra. This is a patch entry, not a normal C-callable API. */
        goto *g_CardPatchContinuation;
    }
}
