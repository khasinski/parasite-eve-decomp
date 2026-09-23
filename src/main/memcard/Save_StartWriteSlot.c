#include "pe1/memcard.h"
#include "pe1/psyq_libc.h"

void Save_StartWriteSlot(int port, int arg_slot) {
    MemCardPortState *state;
    MemCardSaveSlot *slot_state;
    register int slot asm("$16");
    int status;
    int sequence;
    int next_state;
    char *path;

    slot = arg_slot;
    state = &g_MemCardPortStates[port];
    status = state->present;
    if (status != 1) {
        return;
    }

    /* Keep the original slot register for the slot-array address calculation. */
    asm volatile("" : "=r"(slot) : "0"(slot));
    slot_state = &state->slots[slot];
    MemCard_CloseAll();
    Save_BuildHeader();

    sequence = state->sequence;
    state->sequence = ({
        int next_sequence;

        if (slot_state->state == MEMCARD_SLOT_OCCUPIED) {
            next_sequence = sequence;
        } else {
            next_sequence = sequence + 1;
        }
        path = D_8009EE70;
        state->selectedSlot = slot;
        /* Preserve the order of the two metadata stores. */
        asm volatile("" : : : "memory");
        next_sequence;
    });

    Square_Vsprintf(path, D_80092224,
                    state > g_MemCardPortStates,
                    state->slots[(u8)slot].titleStyleFlag + 0x30,
                    (u8)slot + 0x41);
    Save_BuildCardFile(g_MemCardPathForCardFile);

    state->listCursor = slot;
    state->scanCursor = 0;
    slot_state->metadataReady = 0;
    state->managerState = 1;
    state->pendingError = 1;
    state->transferSize = 0x2000;
    state->retryCount = 10;
    g_MemCardActiveState = state;
    g_MemCardActiveBytesRemaining = 0x2000;

    next_state = 4;
    if (slot_state->state == MEMCARD_SLOT_OCCUPIED) {
        next_state = 0x0B;
    }
    state->nextState = next_state;
    slot_state->state = MEMCARD_SLOT_OCCUPIED;
}
