extern int g_SaveTitleStyleFlag;

#include "pe1/memcard.h"

int Save_GetTitleStyleFlag(void) {
    return g_SaveTitleStyleFlag;
}

int MemCard_HasAnyPresentFile(void) {
    unsigned char *base;
    unsigned char *port;
    register unsigned char *end;
    unsigned char *outer_end;
    register unsigned char *entry;
    unsigned char *entry_end;
    int found;
    int state_full;
    int entry_active;
    int present;

    base = (unsigned char *)g_MemCardPortStates;
    end = base + 0x830;
    found = 0;

    if (base < end) {
        state_full = 15;
        entry_active = 1;
        outer_end = end;
        port = base + 0x418;
        do {
            if (port[-0x417] == state_full) {
                entry = base + PE1_OFFSETOF(MemCardPortState, slots);
                if (entry < port) {
                    entry_end = port;
                    do {
                        found = 0;
                        if (((MemCardSaveSlot *)entry)->state == entry_active) {
                            present = ((MemCardSaveSlot *)entry)->titleStyleFlag;
                            found = present != 0;
                        }
                        if (found != 0) {
                            break;
                        }
                        entry += sizeof(MemCardSaveSlot);
                    } while (entry < entry_end);
                }
            }
            base += sizeof(MemCardPortState);
            port += sizeof(MemCardPortState);
            if (base >= outer_end) {
                break;
            }
        } while (found == 0);
    }
    return found;
}
