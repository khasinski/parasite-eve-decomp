#include "pe1/memcard.h"

void MemCard_MarkActivePortState13(void) {
    int portIndex = g_MemCardActivePortOneBased - 1;
    g_MemCardPortStates[portIndex].managerState = 0xD;
}
