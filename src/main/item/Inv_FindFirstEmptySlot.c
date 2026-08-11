#include "pe1/inventory_slots.h"

int Inv_FindFirstEmptySlot(int neededSlots) {
    short *current;
    short *lastStart;
    short *base;
    short *probe;
    int runLength;

    g_InvItemPtr = g_AyaInventoryItems;
    g_InvSlotLimit = Inv_GetAyaSlotLimit();
    g_InvSelectionBits = g_InvSelectionBitStorage;
    g_InvSelectionBitWords = 2;

    current = g_InvItemPtr;
    lastStart = current + g_InvSlotLimit - neededSlots;
    if (current < lastStart + 1) {
        base = current;
        while (current < lastStart + 1) {
            while (*current != 0 && current < lastStart + 1) {
                current++;
            }
            if (current < lastStart + 1) {
                runLength = 1;
                if (runLength >= neededSlots) {
                    return current - base;
                }
                if (current[1] == 0) {
                    runLength = 2;
                    if (runLength >= neededSlots) {
                        return current - base;
                    }
                    if (current[2] == 0) {
                        probe = current + 2;
                        runLength++;
                        while (runLength < neededSlots) {
                            probe++;
                            if (*probe != 0) {
                                runLength--;
                                break;
                            }
                            runLength++;
                        }
                        if (runLength >= neededSlots) {
                            return current - base;
                        }
                    }
                }
            }
            current++;
        }
    }
    return -1;
}
