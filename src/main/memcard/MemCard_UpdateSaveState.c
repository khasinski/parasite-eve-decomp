#include "pe1/memcard_save_state.h"

static inline void MemCard_FormatSlotFileName(MemCardPortState *state,
                                              int slot) {
    Square_Vsprintf(D_8009EE70, D_80092224, state > g_MemCardPortStates,
                    state->slots[slot].titleStyleFlag + '0', slot + 'A');
}

/* After the retry budget runs out, drop the operation and report it. */
#define MEMCARD_RETRY_OR_FAIL(state)                                         \
    if ((state)->retryCount-- <= 0) {                                        \
        if ((state)->present & 1) {                                          \
            MemCard_AbortActiveOperation(state);                             \
            switch ((state)->pendingError) {                                 \
            case 1:                                                          \
                Menu_CreateTwoLineDialog(0x3D, 0x3F);                        \
                break;                                                       \
            case 2:                                                          \
                Menu_CreateNotificationDialog(0x3E, 0);                      \
                break;                                                       \
            }                                                                \
            (state)->pendingError = 0;                                       \
            (state)->managerState = 12;                                      \
        } else {                                                             \
            MemCard_AbortActiveOperation(state);                             \
        }                                                                    \
    }

static inline void MemCard_NoteSaveFile(MemCardPortState *state,
                                        MemCardDirEntry *entry) {
    MemCardSaveSlot *slot;

    if (strncmp(entry->name, &D_80092224[6], 12) == 0) {
        slot = &state->slots[entry->name[19] - 'A'];
        slot->state = MEMCARD_SLOT_OCCUPIED;
        slot->metadataReady = 0;
        slot->titleStyleFlag = entry->name[18] != '0';
        state->hasFiles = 1;
    }
    state->sequence += entry->size >> 13;
}

static inline void MemCard_ApplySavePreview(MemCardSaveSlot *slot,
                                            MemCardSavePreview *preview) {
    if (slot->state == MEMCARD_SLOT_OCCUPIED) {
        slot->currentHp = preview->currentHp;
        slot->maxHp = preview->maxHp;
        slot->levelIndex = preview->levelIndex;
        slot->playTimeMinutes = preview->playTimeMinutes;
        slot->gameTimeMinutes = preview->gameTimeMinutes;
        slot->blendColor = preview->blendColor;
        slot->exGameIndex = preview->exGameIndex;
        slot->progressStage = preview->progressStage;
        slot->mapNumber = preview->mapNumber;
        slot->primaryTitle = preview->primaryTitle;
        slot->alternateTitle = preview->alternateTitle;
    }
    slot->metadataReady = 1;
}

static inline int MemCard_CanStartSave(int port) {
    MemCardPortState *card = &g_MemCardPortStates[port];
    int i;
    int ok;

    for (i = 0; i < 15; i++) {
        if (card->slots[i].state == MEMCARD_SLOT_OCCUPIED) {
            break;
        }
    }
    ok = 0;
    if (i < 15 || (card->sequence < 15 && MemCard_GetDialogMode() != 0)) {
        ok = 1;
    }
    return ok;
}

void MemCard_UpdateSaveState(int port) {
    MemCardPortState *state;
    char device[8];
    MemCardDirEntry entry;
    int i;
    int blocks;
    int fd;
    int count;
    unsigned int candidate;
    u8 cursor;
    u8 next;
    u8 slot;
    s16 size;
    u8 other;
    u8 current;
    MemCardSavePreview *preview;

    state = &g_MemCardPortStates[port];
    switch (state->managerState) {
    case 0:
    case 15:
        break;

    case 1:
        if (state->present != 1) {
            MemCard_AbortActiveOperation(state);
            break;
        }
        current = g_MemCardPortStates[port].cardState;
        if (current == 4) {
            other = g_MemCardPortStates[port == 0].cardState;
            if (other == 0 || other == current) {
                D_800A1838 = 1;
                state->managerState = state->nextState;
            }
        }
        break;

    case 13:
        if (state->present == 5) {
            current = g_MemCardPortStates[port].cardState;
            if (current == 4) {
                other = g_MemCardPortStates[port == 0].cardState;
                if (other == 0 || other == current) {
                    D_800A1838 = 1;
                    state->managerState = 14;
                }
            }
        } else {
            D_800A1864 = -1;
            state->managerState = 0;
        }
        break;

    case 14:
        Square_Vsprintf(device, D_80010F60, port);
        D_800A1864 = format(device) ? 12 : -1;
        state->present &= ~4;
        D_800A1838 = 0;
        state->managerState = 0;
        break;

    case 2:
        if (state->present != 1) {
            MemCard_AbortActiveOperation(state);
            break;
        }
        D_80092230[2] = port + '0';
        state->fileCount = 0;
        state->selectedSlot = 0;
        state->scanCursor = 0;
        state->hasFiles = 0;
        state->pendingError = 0;
        state->sequence = 0;
        current = MEMCARD_SLOT_EMPTY;
        for (i = 14; i >= 0; i--) {
            state->slots[i].state = current;
        }
        if (Scene_CreateEntityNode(D_80092230, &entry) != 0) {
            MemCard_NoteSaveFile(state, &entry);
            while (nextfile(&entry) != 0) {
                MemCard_NoteSaveFile(state, &entry);
            }
            state->retryCount = 0;
        } else {
            state->retryCount--;
        }
        if (state->retryCount > 0) {
            break;
        }
        blocks = state->sequence;
        for (i = 0; i < 15; i++) {
            if (state->slots[i].state == MEMCARD_SLOT_EMPTY) {
                state->slots[i].metadataReady = 1;
            } else {
                blocks--;
            }
        }
        for (i = 14; i >= 0; i--) {
            if (blocks == 0) {
                break;
            }
            if (state->slots[i].state == MEMCARD_SLOT_EMPTY) {
                state->slots[i].state = MEMCARD_SLOT_UNAVAILABLE;
                blocks--;
            }
        }
        state->fileCount = 15;
        if (D_800A186C == 0) {
            state->managerState = 15;
            D_800A1838 = 0;
            break;
        }
        if (MemCard_CanStartSave(port)) {
            state->managerState = 3;
            state->retryCount = 10;
            MenuWidget_RestoreSavedCurrentNode();
            state->listCursor = Menu_CreateSaveSlotListView(
                state - g_MemCardPortStates, state->fileCount);
            break;
        }
        MenuWidget_RestoreSavedCurrentNode();
        Menu_ShowMemCardErrorDialog(port);
        state->managerState = 12;
        D_800A1838 = 0;
        break;

    case 3:
        if (state->present != 1) {
            MemCard_AbortActiveOperation(state);
            break;
        }
        if (state->scanCursor < state->fileCount * 2) {
            for (; state->scanCursor < state->fileCount * 2;
                 state->scanCursor++) {
                cursor = state->scanCursor;
                candidate = state->listCursor +
                            ((cursor & 1) * 2 - 1) * ((cursor + 1) >> 1);
                if (candidate < 15 &&
                    state->slots[candidate].metadataReady == 0) {
                    break;
                }
            }
            next = state->scanCursor;
            if (next < state->fileCount * 2) {
                slot = state->listCursor +
                       ((next & 1) * 2 - 1) * ((next + 1) >> 1);
            } else {
                slot = 0xFF;
            }
        } else {
            slot = 0xFF;
        }
        state->selectedSlot = slot;
        if (slot != 0xFF) {
            MemCard_FormatSlotFileName(state, slot);
            if ((state->fileDescriptor = open(D_8009EE70, 1)) >= 0) {
                state->managerState = 8;
                state->transferData = D_800A1720[port].bytes;
                state->transferSize = 0x80;
                state->retryCount = 30;
                if (state->slots[state->selectedSlot].state ==
                    MEMCARD_SLOT_OCCUPIED) {
                    lseek(state->fileDescriptor, 0x100, 0);
                }
            } else {
                MEMCARD_RETRY_OR_FAIL(state);
            }
        } else {
            state->managerState = 12;
            D_800A1838 = 0;
        }
        break;

    case 4:
        if (state->present != 1) {
            MemCard_AbortActiveOperation(state);
            break;
        }
        state->slots[state->selectedSlot].titleStyleFlag = D_800A1704;
        MemCard_FormatSlotFileName(state, state->selectedSlot);
        if ((fd = state->fileDescriptor = open(D_8009EE70, 0x10200)) >= 0) {
            close(fd);
            state->fileDescriptor = -1;
            state->managerState = 6;
            state->retryCount = 10;
        } else {
            MEMCARD_RETRY_OR_FAIL(state);
        }
        break;

    case 5:
        if (state->present != 1) {
            MemCard_AbortActiveOperation(state);
            break;
        }
        MemCard_FormatSlotFileName(state, state->selectedSlot);
        if ((state->fileDescriptor = open(D_8009EE70, 1)) >= 0) {
            state->retryCount = 30;
            state->managerState = 7;
        } else {
            MEMCARD_RETRY_OR_FAIL(state);
        }
        break;

    case 6:
        if (state->present != 1) {
            MemCard_AbortActiveOperation(state);
            break;
        }
        MemCard_FormatSlotFileName(state, state->selectedSlot);
        if ((state->fileDescriptor = open(D_8009EE70, 2)) >= 0) {
            state->managerState = 9;
            state->transferData = D_8009EED0;
            state->retryCount = 30;
        } else {
            MEMCARD_RETRY_OR_FAIL(state);
        }
        break;

    case 8:
        if (state->present != 1) {
            MemCard_AbortActiveOperation(state);
            break;
        }
        size = state->transferSize;
        if (state->transferSize > 0x80) {
            size = 0x80;
        }
        count = read(state->fileDescriptor, state->transferData, size);
        if (count > 0) {
            state->transferData += count;
            state->transferSize -= count;
            if (state->transferSize <= 0) {
                state->managerState = 10;
            }
        } else {
            MEMCARD_RETRY_OR_FAIL(state);
        }
        break;

    case 7:
        if (state->present != 1) {
            MemCard_AbortActiveOperation(state);
            break;
        }
        size = state->transferSize;
        if (state->transferSize > 0x400) {
            size = 0x400;
        }
        count = read(state->fileDescriptor, state->transferData, size);
        if (count > 0) {
            state->transferData += count;
            state->transferSize -= count;
            if (state->transferSize <= 0) {
                close(state->fileDescriptor);
                state->fileDescriptor = -1;
                state->managerState = 12;
                state->pendingError = 0;
                D_800A1854 = 0;
                D_800A1838 = 0;
                Save_LoadCardFileIntoRuntime();
            }
        } else {
            MEMCARD_RETRY_OR_FAIL(state);
        }
        break;

    case 9:
        if (state->present != 1) {
            MemCard_AbortActiveOperation(state);
            break;
        }
        size = state->transferSize;
        if (state->transferSize > 0x400) {
            size = 0x400;
        }
        count = write(state->fileDescriptor, state->transferData, size);
        if (count > 0) {
            state->transferData += count;
            state->transferSize -= count;
            if (state->transferSize <= 0) {
                close(state->fileDescriptor);
                state->fileDescriptor = -1;
                state->managerState = 3;
                state->pendingError = 0;
                D_800A1854 = 0;
                Menu_DestroyMemCardProgressWidget();
                Menu_CreateNotificationDialog(0x53, 0);
            }
        } else {
            MEMCARD_RETRY_OR_FAIL(state);
        }
        break;

    case 10:
        if (state->present != 1) {
            MemCard_AbortActiveOperation(state);
            break;
        }
        close(state->fileDescriptor);
        state->fileDescriptor = -1;
        MemCard_ApplySavePreview(&state->slots[state->selectedSlot],
                                 &D_800A1720[port].preview);
        state->scanCursor++;
        state->managerState =
            state->scanCursor < state->fileCount * 2 ? 3 : 12;
        if (state->managerState == 12) {
            D_800A1838 = 0;
        }
        break;

    case 11:
        if (state->present != 1) {
            MemCard_AbortActiveOperation(state);
            break;
        }
        MemCard_FormatSlotFileName(state, state->selectedSlot);
        if ((fd = state->fileDescriptor = open(D_8009EE70, 1)) >= 0) {
            close(fd);
            state->fileDescriptor = -1;
            MemCard_FormatSlotFileName(state, state->selectedSlot);
            if (erase(D_8009EE70) != 0) {
                state->managerState = 4;
                state->retryCount = 10;
            } else {
                MEMCARD_RETRY_OR_FAIL(state);
            }
        } else {
            MEMCARD_RETRY_OR_FAIL(state);
        }
        break;

    case 12:
        if (state->present != 1) {
            MemCard_AbortActiveOperation(state);
        }
        break;
    }
}
