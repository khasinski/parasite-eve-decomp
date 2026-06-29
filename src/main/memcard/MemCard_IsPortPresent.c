#include "pe1/memcard.h"

int MemCard_IsPortPresent(int port) {
    return g_MemCardPortStates[port].present & 1;
}
