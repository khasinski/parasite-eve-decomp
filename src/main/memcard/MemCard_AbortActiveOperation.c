#include "pe1/memcard.h"
#include "pe1/menu_inventory.h"
#include "pe1/psyq_bios.h"
#include "pe1/psyq_libc.h"

void MemCard_AbortActiveOperation(MemCardPortState *state) {
    int state_index;
    int retry;
    int fd;
    int slot;
    int is_second;
    MemCardPortState *base;
    char path[0x20];

    if (g_MemCardLoadSucceeded != 0) {
        Save_CancelUiFlow();
        return;
    }

    if (state->fileDescriptor >= 0) {
        close(state->fileDescriptor);
        state->fileDescriptor = -1;
    }

    if (state->managerState == 9) {
        char *name_buf;

        base = g_MemCardPortStates;
        is_second = base < state;
        state_index = state - base;
        name_buf = D_8009EE70;
        /* Scheduling dependency for the retail argument setup. */
        asm("" : "=r"(name_buf) : "0"(name_buf), "r"(is_second));
        retry = 0;
        slot = state->selectedSlot;

        Square_Vsprintf(name_buf, D_80092224, is_second,
                        state->slots[slot].titleStyleFlag + 0x30, slot + 0x41);
        Square_Vsprintf(path, D_80010F4C, state_index, D_8009EE70);

        do {
            fd = open(path, 1);
            if (fd != -1) {
                break;
            }
            retry++;
        } while (retry < 10);

        if (retry < 10) {
            close(fd);
            erase(path);
        }
    }

    state_index = state - g_MemCardPortStates;
    Menu_CloseSaveSlotListView(state_index);
    state->managerState = 0;
    state->hasFiles = 0;
    g_MemCardActiveState = 0;
    D_800A1838 = 0;
    MenuWidget_RestoreSavedCurrentNode();
}
