#include "common.h"
#include "pe1/save_blob.h"

extern u8 D_800B8868[];
extern u8 D_800B88C8[];
extern u8 D_8009EED0[];
extern unsigned short D_80010F48;
extern int D_800A5D50;
extern u8 *g_SaveIoCursor;
extern SaveBytes12E4 g_SaveRuntimeState;
extern int D_800C0DE8;

void bzero(void *dst, int len);
char *strcpy(char *dst, char *src);
u8 *Str_ResolveDataPtr(char *path);
u8 *Save_FormatTitle(int chapter, int playtime_seconds);
void Save_SerializeTail(void);

int Save_BuildCardFile(char *path) {
    u8 *resolved;
    u8 *file;
    u32 crc;

    resolved = Str_ResolveDataPtr(path);
    file = D_800B8868;

    D_800A5D50 = 0x2000;
    bzero(file, 0x100);

    *(short *)(file + 0) = D_80010F48;
    file[2] = 0x11;
    file[3] = 1;
    strcpy((char *)(file + 4), (char *)Save_FormatTitle(path[0x18] - 0x40, D_800C0DE8));

    __builtin_memcpy(D_800B88C8, resolved + 0x14, 0x20);
    __builtin_memcpy(file + 0x80, resolved + 0x40, 0x80);

    bzero(D_8009EED0, 0x2000);
    g_SaveIoCursor = D_8009EED0;
    __builtin_memcpy(g_SaveIoCursor, D_800B8868, 0x100);
    g_SaveIoCursor += 0x100;
    __builtin_memcpy(g_SaveIoCursor, (u8 *)&g_SaveRuntimeState, 0x12E4);
    g_SaveIoCursor += 0x12E4;

    Save_SerializeTail();

    {
        u8 *data = D_8009EED0;
        u32 checksum = 0xFFFF;
        u32 i = 0;
        do {
            u32 bit = 0;
            checksum ^= data[i & 0xFFFF] << 8;
            do {
                if (checksum & 0x8000) {
                    checksum = (checksum << 1) ^ 0x1021;
                } else {
                    checksum <<= 1;
                }
                bit++;
            } while ((bit & 0xFFFF) < 8);
            i++;
        } while ((i & 0xFFFF) < 0x2000);
        crc = ~checksum & 0xFFFF;
    }
    __builtin_memcpy(g_SaveIoCursor, &crc, 4);
    g_SaveIoCursor += 4;

    return 0;
}
#include "pe1/memcard_save_state.h"

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
#include "common.h"
#include "pe1/memcard.h"
extern u8 g_MemCardFileBuffer[];
void bzero(void *dst, int len);
void MemCard_CloseAll(void);

void MenuWidget_NavScrollTo(int selected_base);
void Inv_SetActiveList(int arg0, int arg1);

int Save_StartReadSlot(int port, int slot)
{
  register u8 *buffer;
  MemCardPortState *state;
  int saved_slot;
  state = &g_MemCardPortStates[port];
  saved_slot = slot;
  if (state->present == 1)
  {
    MemCard_CloseAll();
 do { buffer = g_MemCardFileBuffer; } while (0);
    state->transferSize = 0x2000;
    state->retryCount = 10;
    state->pendingError = 2;
    state->managerState = 1;
    state->nextState = 5;
    state->transferData = buffer;
    state->selectedSlot = saved_slot;
    g_MemCardActiveState = state;
    g_MemCardActiveBytesRemaining = 0x2000;
    bzero(buffer, 0x2000);
  }
  return 0;
}

void Save_CancelUiFlow(void) {
    MenuWidget_NavScrollTo(0x26);
    MenuWidget_NavScrollTo(0x25);
    MenuWidget_NavScrollTo(0x24);
    Inv_SetActiveList(0xC, 0);
}

#include "common.h"
#include "pe1/save_blob.h"

extern SaveBytes12E4 g_MemCardSaveStateBuffer;
extern SaveBytes12E4 g_SaveRuntimeState;
extern u8 g_MemCardFileBuffer[];
extern u8 *g_SaveIoCursor;
extern int g_MemCardLoadSucceeded;

void Save_DeserializeTail(void);
void Save_RestoreHeader(void);
void Menu_DestroyMemCardProgressWidget(void);
void Menu_CreateNotificationDialog(int arg0, int arg1);
void Menu_SetDeferredCallback(void (*callback)(void));
void Menu_CreateTwoLineDialog(int arg0, int arg1);
void Save_CancelUiFlow(void);

void Save_LoadCardFileIntoRuntime(void) {
    SaveBytes4 expected_crc;
    u8 *data;
    u32 crc;
    u32 i;
    u32 bit;
    u8 *cursor;

    g_SaveIoCursor = (u8 *)&g_MemCardSaveStateBuffer;
    g_SaveRuntimeState = g_MemCardSaveStateBuffer;
    g_SaveIoCursor += 0x12E4;
    Save_DeserializeTail();

    data = g_MemCardFileBuffer;
    crc = 0xFFFF;
    i = 0;
    cursor = g_SaveIoCursor;
    expected_crc = *(SaveBytes4 *)cursor;
    g_SaveIoCursor += 4;
    *(u32 *)cursor = 0;

    do {
        crc ^= data[i & 0xFFFF] << 8;
        bit = 0;
        do {
            if (crc & 0x8000) {
                crc = (crc << 1) ^ 0x1021;
            } else {
                crc <<= 1;
            }
            bit++;
        } while ((bit & 0xFFFF) < 8);
        i++;
    } while ((i & 0xFFFF) < 0x2000);

    if ((~crc & 0xFFFF) == *(u32 *)expected_crc.b) {
        Save_RestoreHeader();
        g_MemCardLoadSucceeded = 1;
        Menu_DestroyMemCardProgressWidget();
        Menu_CreateNotificationDialog(0x54, 0);
        Menu_SetDeferredCallback(Save_CancelUiFlow);
    } else {
        Menu_DestroyMemCardProgressWidget();
        Menu_CreateTwoLineDialog(0x55, 0x56);
    }
}
#include "pe1/memcard.h"

extern int D_800A1858;

int MemCard_GetActiveProgressBlocks(void) {
    int blocks;
    int result;
    MemCardPortState *active = g_MemCardActiveState;

    if (active != 0) {
        blocks = (D_800A1858 - active->transferSize + 0x400) >> 10;
        result = 8;
        if (blocks < 9) {
            result = blocks;
        }
    } else {
        result = 0;
    }

    return result;
}


/* Byte aliases map fileCount and the first slot fields in MemCardPortState. */
extern u8 D_800A0ED6[];
extern u8 D_800A0EF1[];
extern u8 D_800A0EF0[];

MemCardSaveSlot *MemCard_GetSlot(unsigned int port, int slot) {
    int port_offset;
    int slot_offset;
    u8 *base;

    if (port >= 2U) {
        return 0;
    }
    if (slot < 0) {
        return 0;
    }

    port_offset = ((((port << 5) + port) << 2) - port) << 3;
    if (slot >= D_800A0ED6[port_offset]) {
        return 0;
    }

    slot_offset = ((slot << 4) + slot) << 2;
    if (D_800A0EF1[slot_offset + port_offset] == 0) {
        return 0;
    }

    base = D_800A0EF0 + port_offset;
    return (MemCardSaveSlot *)(base + slot_offset);
}

extern int D_800A12F8;
extern int D_800A0EE0;
extern int g_MemCardServicedPort;
extern int g_MemCardInfoPollCountdown;
extern int g_MemCardConnectDebounce;
extern int D_800A184C;
extern int D_800A1848;
extern int g_McOpPending;
extern int g_MemCardActivePortOneBased;
extern int g_MemCardSavePollTimeout;
extern int g_MemCardActivePromptPending;
extern int g_SaveTitleStyleFlag;
extern int g_MemCardLoadSucceeded;
extern int g_MemCardReadContext;

void bzero(void *ptr, int size);

void MemCard_InitState(void) {
    bzero(g_MemCardPortStates, sizeof(MemCardPortState) * 2);
    D_800A12F8 = -1;
    D_800A0EE0 = -1;
    g_MemCardServicedPort = 0;
    g_MemCardInfoPollCountdown = 0;
    g_MemCardConnectDebounce = 0;
    D_800A184C = 0;
    D_800A1848 = 0;
    g_McOpPending = 0;
    g_MemCardActiveState = 0;
    g_MemCardActivePortOneBased = 0;
    g_MemCardSavePollTimeout = 0;
    g_MemCardActivePromptPending = 0;
    g_SaveTitleStyleFlag = 0;
    g_MemCardLoadSucceeded = 0;
    g_MemCardReadContext = 0;
}


#include "common.h"
extern u8 D_800A0EDC[];
extern u8 g_Slot2QuickerSave[];
extern int g_MemCardActivePortOneBased;
extern int g_MemCardSavePollTimeout;
extern int g_MemCardActivePromptPending;
void MemCard_StepPortState(int arg0);
void Menu_CreateNotificationDialog(int arg0, int arg1);
void Menu_CloseNotificationDialogs(void);
void Menu_SetDeferredCallback(void (*callback)(void));
void Menu_DestroyMemCardProgressWidget(void);
void Menu_NavToSaveConfirmDialog(void);
void MemCard_UpdateSaveState(int port);
void MemCard_ClearActivePrompt(void);
void MemCard_StartActivePortRead(void);

#include "pe1/memcard.h"

void MemCard_UpdateSavePolling(void)
{
  int port;
  u8 *state;
  int timeout;
  int activePort;
  int activeState;
  MemCard_StepPortState(1);
  MemCard_StepPortState(0);
  if (g_MemCardSavePollTimeout != 0)
  {
    activePort = g_MemCardActivePortOneBased - 1;
    activeState = D_800A0EDC[activePort * 0x418];
    if (activeState == 4)
    {
      goto active_state_ok;
    }
    {
      register int activeStateReady;
      activeStateReady = 1;
      if (activeState == activeStateReady)
      {
        goto active_state_ok;
      }
    }
    g_MemCardSavePollTimeout = -2;
    active_state_ok:
    timeout = g_MemCardSavePollTimeout;

    timeout -= timeout > 0;
    g_MemCardSavePollTimeout = timeout;
    if (timeout <= 0)
    {
      Menu_DestroyMemCardProgressWidget();
      if (g_MemCardSavePollTimeout == (-1))
      {
        goto timeout_expired;
      }
      if (g_MemCardSavePollTimeout != 0)
      {
        goto timeout_cancel;
        port = 1;
      }
      Menu_CreateNotificationDialog(0x52, 0);
      Menu_SetDeferredCallback(MemCard_StartActivePortRead);
      goto timeout_prompt_pending;
      timeout_expired:
      Menu_CreateNotificationDialog(0x3C, 0);

      Menu_SetDeferredCallback(MemCard_ClearActivePrompt);
      timeout_prompt_pending:
      g_MemCardActivePromptPending = 1;

      goto timeout_clear;
      timeout_cancel:
      MemCard_ClearActivePrompt();

      timeout_clear:
      g_MemCardSavePollTimeout = 0;

      port = 1;
    }
  }
  port = 1;
  state = g_Slot2QuickerSave;
  do
  {
    if (g_MemCardActivePortOneBased == (port + 1))
    {
      if (((*state) & 1) == 0)
      {
        Menu_NavToSaveConfirmDialog();
        Menu_CloseNotificationDialogs();
        MemCard_ClearActivePrompt();
      }
    }
    MemCard_UpdateSaveState(port);
    port--;
    state -= 0x418;
  }
  while (port >= 0);
}

int MemCard_IsPortPresent(int port) {
    return g_MemCardPortStates[port].present & 1;
}


#include "pe1/memcard.h"

int close(int fd);

void MemCard_CloseAll(void) {
    unsigned char *base;
    unsigned char *state;
    unsigned char *end;
    int stateOpen;
    int stateBusy;
    int closedFd;
    int stateClosed;

    base = (unsigned char *)g_MemCardPortStates;
    if (base < base + 0x830) {
        stateOpen = 8;
        stateBusy = 10;
        closedFd = -1;
        stateClosed = 12;
        state = base + 1;
        end = base + 0x831;
        do {
            if ((*state == stateOpen) || (*state == stateBusy)) {
                close(*(volatile int *)(state + 0xB));
                *(volatile int *)(state + 0xB) = closedFd;
                *state = stateClosed;
            }
            state += 0x418;
        } while (state < end);
    }
}


#include "pe1/memcard.h"

void Menu_StepItemGrid2(void);

extern int g_MemCardActivePortOneBased;

int MemCard_CheckPresent(int port) {
    int present;

    present = g_MemCardPortStates[port].present & 4;
    if (present != 0) {
        if (g_MemCardActivePortOneBased == 0) {
            g_MemCardActivePortOneBased = port + 1;
            Menu_StepItemGrid2();
        }
    }
    return present == 0;
}

int MemCard_GetActivePort(void)
{
    return g_MemCardActivePortOneBased - 1;
}


#include "pe1/memcard.h"

void MemCard_MarkActivePortState13(void) {
    int portIndex = g_MemCardActivePortOneBased - 1;
    g_MemCardPortStates[portIndex].managerState = 0xD;
}


extern volatile int g_MemCardActivePortOneBased;
extern volatile int g_MemCardActivePromptPending;

void MemCard_ClearActivePrompt(void) {
    g_MemCardActivePortOneBased = 0;
    g_MemCardActivePromptPending = 0;
}


#include "common.h"
#include "pe1/memcard.h"
extern int g_MemCardActivePortOneBased;
extern int g_MemCardActivePromptPending;

void MemCard_StartRead(int port, int arg1);

extern int g_MemCardReadContext;

void MenuWidget_SaveAndSetCurrentNode(int arg0);

void MemCard_StartActivePortRead(void) {
    MemCard_StartRead(g_MemCardActivePortOneBased - 1, 1);
    g_MemCardActivePortOneBased = 0;
    g_MemCardActivePromptPending = 0;
}

int MemCard_GetPortSequence(int arg0) {
    return g_MemCardPortStates[arg0].sequence;
}

void MemCard_StartRead(int port, int arg1) {
    MemCardPortState *state = &g_MemCardPortStates[port];

    if (state->managerState == 0 || state->managerState == 12) {
        state->managerState = 1;
        state->nextState = 2;
        state->retryCount = 10;
        MenuWidget_SaveAndSetCurrentNode(0);
        g_MemCardReadContext = arg1;
    }
}


#include "pe1/memcard.h"
extern unsigned char D_800A12ED;
extern int g_McOpPending;

int close(int fd);

void MemCard_CloseAllAndResetState(void) {
    unsigned char *base;
    unsigned char *state;
    unsigned char *end;
    int stateOpen;
    int stateBusy;
    int closedFd;
    int stateClosed;

    base = (unsigned char *)g_MemCardPortStates;
    if (base < base + 0x830) {
        stateOpen = 8;
        stateBusy = 10;
        closedFd = -1;
        stateClosed = 12;
        state = base + 1;
        end = base + 0x831;
        do {
            if ((*state == stateOpen) || (*state == stateBusy)) {
                close(*(volatile int *)(state + 0xB));
                *(volatile int *)(state + 0xB) = closedFd;
                *state = stateClosed;
            }
            state += 0x418;
        } while (state < end);
    }

    D_800A12ED = 0;
    g_MemCardPortStates[0].managerState = 0;
    g_McOpPending = 0;
}


#include "pe1/memcard.h"

extern int g_McOpPending;

extern void (*g_MemCardDelayedCallback)(void);
extern int g_MemCardDelayedCallbackTimer;

int MemCard_IsPortTransferState(int arg0) {
    u8 value = g_MemCardPortStates[arg0].managerState;

    return (value == 3) || (value == 8) || (value == 10);
}

int MemCard_IsOperationPending(void) {
    return g_McOpPending;
}

void MemCard_ClearDelayedCallback(void) {
    g_MemCardDelayedCallback = 0;
    g_MemCardDelayedCallbackTimer = 0;
}


extern void (*g_MemCardDelayedCallback)(void);
extern int g_MemCardDelayedCallbackTimer;

void MemCard_SetDelayedCallback(void (*callback)(void)) {
    g_MemCardDelayedCallback = callback;
    g_MemCardDelayedCallbackTimer = 1;
}

void MemCard_DelayedCallback(void) {
    void (*callback)(void);

    callback = g_MemCardDelayedCallback;
    if (callback != 0) {
        g_MemCardDelayedCallbackTimer++;
        if (g_MemCardDelayedCallbackTimer == 4) {
            callback();
            g_MemCardDelayedCallback = 0;
            g_MemCardDelayedCallbackTimer = 0;
        }
    }
}


extern void (*g_MemCardDelayedCallback)(void);

extern int g_MemCardEventF400Spec0004Flag;
extern int g_MemCardRemovedEventPending;
extern int g_MemCardEventF400Spec2000Flag;
extern int g_MemCardEventF000Spec0004Flag;
extern int g_MemCardEventF000Spec8000Flag;
extern int g_MemCardEventF000Spec2000Flag;

int MemCard_HasDelayedCallback(void) {
    return g_MemCardDelayedCallback != 0;
}

int MemCard_OnEventF400Spec0004(void)
{
    g_MemCardEventF400Spec0004Flag = 1;
    return 0;
}

int MemCard_OnEventF400Spec8000(void)
{
    g_MemCardRemovedEventPending = 1;
    return 0;
}

int MemCard_OnEventF400Spec0100(void)
{
    g_MemCardRemovedEventPending = 1;
    return 0;
}

int MemCard_OnEventF400Spec2000(void)
{
    g_MemCardEventF400Spec2000Flag = 1;
    return 0;
}

int MemCard_OnEventF000Spec0004(void)
{
    g_MemCardEventF000Spec0004Flag = 1;
    return 0;
}

int MemCard_OnEventF000Spec8000(void)
{
    g_MemCardEventF000Spec8000Flag = 1;
    return 0;
}

int MemCard_OnEventF000Spec0100(void)
{
    g_MemCardEventF000Spec8000Flag = 1;
    return 0;
}

int MemCard_OnEventF000Spec2000(void)
{
    g_MemCardEventF000Spec2000Flag = 1;
    return 0;
}
