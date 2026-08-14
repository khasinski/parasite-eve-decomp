#include "pe1/cdrom_callback_internal.h"

void CdRom_SetReadyCallback(unsigned int value) {
    g_DsCallbackSetPage = (DsCallbackSetPage *)0x800A0000;
    g_DsCallbackSetPage->ready = value;
}
