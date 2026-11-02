#include "common.h"
#include "pe1/game_timers.h"
#include "pe1/geom_state.h"
#include "pe1/inventory_slots.h"
#include "pe1/memcard.h"
#include "pe1/memcard_save_state.h"
#include "pe1/menu_notification_dialog.h"
#include "pe1/menu_widget.h"
#include "pe1/psyq_bios.h"
#include "pe1/resident_tables.h"
#include "pe1/save.h"
#include "pe1/save_blob.h"
#include "pe1/save_tail_state.h"
#include "pe1/script_queue.h"

/* The save tail is a packed byte stream: each record is copied as an
 * unaligned byte block at the IO cursor, which then advances past it. */
#define SAVE_PUT(type, src)                                \
    *(type *)g_SaveIoCursor = *(type *)(src);              \
    g_SaveIoCursor += sizeof(type)

void Save_SerializeTail(void) {
    SAVE_PUT(SaveBytes800, g_ScriptVariableBank);
    SAVE_PUT(SaveBytes4, &D_8009CDA8);
    SAVE_PUT(SaveBytes4, &g_SceneDispatchToken);
    SAVE_PUT(SaveBytes4, &g_GameStateFlags);
    SAVE_PUT(SaveBytes4, &g_GameState.saved_state_004);
    SAVE_PUT(SaveBytes2, &g_GameState.room_type);
    SAVE_PUT(SaveBytes2, &g_GameState.requested_entity_bank);
    SAVE_PUT(SaveBytes2, &g_GameState.current_story_day);
    SAVE_PUT(SaveU8, &g_GameState.story_day_flags);
    SAVE_PUT(SaveU8, &g_ScreenTint.fadeMode);
    SAVE_PUT(SaveBytes70, &g_AyaBattleState);
    SAVE_PUT(SaveBytes18, &g_SavedBattleStateTail);
    SAVE_PUT(SaveBytes8, &g_BattleEquipStateBlock);
}

#define SAVE_GET(type, dst)                                \
    *(type *)(dst) = *(type *)g_SaveIoCursor;              \
    g_SaveIoCursor += sizeof(type)

void Save_DeserializeTail(void) {
    SAVE_GET(SaveBytes800, g_ScriptVariableBank);
    SAVE_GET(SaveBytes4, &D_8009CDA8);
    SAVE_GET(SaveBytes4, &g_SceneDispatchToken);
    SAVE_GET(SaveBytes4, &g_GameStateFlags);
    SAVE_GET(SaveBytes4, &g_GameState.saved_state_004);
    SAVE_GET(SaveBytes2, &g_GameState.room_type);
    SAVE_GET(SaveBytes2, &g_GameState.requested_entity_bank);
    SAVE_GET(SaveBytes2, &g_GameState.current_story_day);
    SAVE_GET(SaveU8, &g_GameState.story_day_flags);
    SAVE_GET(SaveU8, &g_ScreenTint.fadeMode);
    SAVE_GET(SaveBytes70, &g_AyaBattleState);
    SAVE_GET(SaveBytes18, &g_SavedBattleStateTail);
    SAVE_GET(SaveBytes8, &g_BattleEquipStateBlock);
    g_GameStateFlags &= 0xFFFF2679;
    D_8009CDA8 &= ~9;
}

void Save_SetTitleStyleFlag(int value) {
    g_SaveTitleStyleFlag = value;
}

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

void Save_SprintfSjis(unsigned char *dst, unsigned char *fmt)
{
    SaveSjisFormatArgument *args;
    int ch; /* also holds each Shift-JIS digit ch */
    unsigned char *out;
    int value;
    unsigned char *text;
    int byte;

    out = dst;
    ch = *fmt;
    args = (SaveSjisFormatArgument *)&g_SaveSjisFormatArguments;
    while (ch != 0) {
        ch &= 0xFF;
        fmt++;
        if (ch == '%') {
            ch = *fmt++;
            switch (ch) {
            case 'd':
                value = args++->number;
                ch = (value / 10) % 10 + 0x824Fu;
                *out++ = (unsigned int)ch >> 8;
                *out++ = (unsigned char)ch;
                ch = value % 10 + 0x824Fu;
                *out++ = (unsigned int)ch >> 8;
                *out++ = (unsigned char)ch;
                break;
            case 'D':
                value = args++->number;
                ch = value % 10 + 0x824Fu;
                *out++ = (unsigned int)ch >> 8;
                *out++ = (unsigned char)ch;
                break;
            case 's':
                text = args++->text;
                if (text == 0) break;
                byte = *text;
                if (byte == 0) break;
                do {
                    text++;
                    *out++ = byte;
                    byte = *text;
                } while (byte != 0);
                break;
            default:
                break;
            }
        } else {
            *out++ = *(fmt - 1);
        }
        ch = *fmt;
    }
    *out = 0;
}

typedef unsigned char u8_1;

u8 *Save_FormatTitle(int chapter, int playtime_seconds) {
    int minutes_total;
    int hours;
    int minutes;
    int seconds;
    u8 *value;

    bzero(g_SaveTitleBuffer, 0x44);

    hours = playtime_seconds / 3600;
    minutes_total = playtime_seconds / 60;
    minutes = minutes_total - (hours * 60);
    seconds = playtime_seconds - (minutes_total * 60);

    g_SaveSjisFormatArguments.chapter = chapter;
    g_SaveSjisFormatArguments.hours = hours;
    g_SaveSjisFormatArguments.minutes = minutes;
    g_SaveSjisFormatArguments.seconds = seconds;

    if (g_SaveTitleStyleFlag != 0) {
        value = Text_GetMapLocationName(-1);
        g_SaveSjisFormatArguments.primary.text = value;
        Save_SprintfSjis(g_SaveTitleBuffer, g_SaveTitleFormatShortPtr);
    } else {
        g_SaveSjisFormatArguments.primary.number =
            Save_ClassifyPlaytime(g_PlayTimeFrameCounter);
        value = Text_GetMapLocationName(Save_GetCurrentMapNumber());
        g_SaveSjisFormatArguments.secondary.text = value;
        Save_SprintfSjis(g_SaveTitleBuffer, g_SaveTitleFormatLongPtr);
    }

    return g_SaveTitleBuffer;
}

u32 Save_CalcCrc16(u32 size, u8_1 *data) {
    u32 crc = 0xFFFF;
    u32 i = 0;

    size &= 0xFFFF;
    if (size != 0) {
        do {
            u32 bit = 0;

            crc ^= data[i & 0xFFFF] << 8;
            do {
                if (crc & 0x8000) {
                    crc = (crc << 1) ^ 0x1021;
                } else {
                    crc <<= 1;
                }
                bit++;
            } while ((bit & 0xFFFF) < 8);
            i++;
        } while ((i & 0xFFFF) < size);
    }

    return ~crc & 0xFFFF;
}

void MemCard_ClearPort1Events(void) {
    TestEvent(g_MemCardEventHandles[MEMCARD_EVENT_HW_IO_END]);
    TestEvent(g_MemCardEventHandles[MEMCARD_EVENT_HW_ERROR]);
    TestEvent(g_MemCardEventHandles[MEMCARD_EVENT_HW_TIMEOUT]);
    TestEvent(g_MemCardEventHandles[MEMCARD_EVENT_HW_NEW_CARD]);
    g_MemCardEventF400Spec2000Flag = 0;
    g_MemCardRemovedEventPending = 0;
    g_MemCardEventF400Spec0004Flag = 0;
}

void MemCard_ClearEvents(void) {
    TestEvent(g_MemCardEventHandles[MEMCARD_EVENT_SW_IO_END]);
    TestEvent(g_MemCardEventHandles[MEMCARD_EVENT_SW_ERROR]);
    TestEvent(g_MemCardEventHandles[MEMCARD_EVENT_SW_TIMEOUT]);
    TestEvent(g_MemCardEventHandles[MEMCARD_EVENT_SW_NEW_CARD]);
    g_MemCardEventF000Spec2000Flag = 0;
    g_MemCardEventF000Spec8000Flag = 0;
    g_MemCardEventF000Spec0004Flag = 0;
}

int MemCard_PollTransferDelay(void) {
    int *pending;
    int count;
    int one;
    int reset;
    int value;
    /* Shared scratch for the pending-port test and the timer clamp. */
    int clamped;
    int final_value;

    count = 0;
    reset = 12;
    one = 1;
    pending = g_MemCardConnectPending;
    value = g_MemCardConnectDebounce;
    g_MemCardConnectDebounce = value - (value > 0);

    do {
        if (g_MemCardPortStates[count].cardState - 2U < 2U) {
            g_MemCardConnectDebounce = reset;
            *pending = one;
        }
        pending++;
        count++;
    } while (count < 2);

    pending = g_MemCardConnectPending;
    __asm__ volatile("" : "=r"(pending) : "0"(pending));
    clamped = pending[0] != 0;
    if (clamped && g_MemCardPortStates[0].cardState != 4) {
    } else if (*(pending + 1) != 0 && g_MemCardPortStates[1].cardState != 4) {
    } else {
        final_value = g_MemCardConnectDebounce;
        clamped = 4;
        if (final_value < 5) {
            clamped = final_value;
        }
        g_MemCardConnectDebounce = clamped;
        *(pending + 1) = 0;
        pending[0] = 0;
    }
    final_value = g_MemCardConnectDebounce > 0;
    return final_value;
}

void _card_clear(int arg0);
void _card_info(int arg0);
void _card_load(int arg0);
int Menu_IsMemCardDialogOpen(void);
void MemCard_StartRead(int port, int arg1);

void MemCard_StepPortState(int port) {
    MemCardPortState *state;
    int value;

    state = &g_MemCardPortStates[port];

    switch (state->cardState) {
    case 0:
        state->present = 0;
        break;

    case 1:
        if (g_MemCardEventF400Spec0004Flag != 0) {
            g_MemCardEventF400Spec0004Flag = 0;
            if ((state->present & 1) == 0) {
                TestEvent(g_MemCardEventHandles[MEMCARD_EVENT_SW_IO_END]);
                TestEvent(g_MemCardEventHandles[MEMCARD_EVENT_SW_ERROR]);
                TestEvent(g_MemCardEventHandles[MEMCARD_EVENT_SW_TIMEOUT]);
                TestEvent(g_MemCardEventHandles[MEMCARD_EVENT_SW_NEW_CARD]);
                g_MemCardEventF000Spec2000Flag = 0;
                g_MemCardEventF000Spec8000Flag = 0;
                g_MemCardEventF000Spec0004Flag = 0;
                _card_clear(port << 4);
                state->cardState = 2;
                return;
            }
            state->cardState = 4;
            g_MemCardInfoPollCountdown = 0;
            g_MemCardServicedPort = g_MemCardServicedPort < 1;
            return;
        }
        if (g_MemCardRemovedEventPending != 0) {
            g_MemCardRemovedEventPending = 0;
            state->cardState = 0;
            state->present &= ~4;
            g_MemCardInfoPollCountdown = 0;
            g_MemCardServicedPort = g_MemCardServicedPort < 1;
            return;
        }
        if (g_MemCardEventF400Spec2000Flag == 0) {
            return;
        }
        g_MemCardEventF400Spec2000Flag = 0;
        TestEvent(g_MemCardEventHandles[MEMCARD_EVENT_SW_IO_END]);
        TestEvent(g_MemCardEventHandles[MEMCARD_EVENT_SW_ERROR]);
        TestEvent(g_MemCardEventHandles[MEMCARD_EVENT_SW_TIMEOUT]);
        TestEvent(g_MemCardEventHandles[MEMCARD_EVENT_SW_NEW_CARD]);
        g_MemCardEventF000Spec2000Flag = 0;
        g_MemCardEventF000Spec8000Flag = 0;
        g_MemCardEventF000Spec0004Flag = 0;
        _card_clear(port << 4);
        state->cardState = 2;
        return;

    case 2:
        if (g_MemCardEventF000Spec0004Flag != 0) {
            g_MemCardEventF000Spec0004Flag = 0;
            TestEvent(g_MemCardEventHandles[MEMCARD_EVENT_HW_IO_END]);
            TestEvent(g_MemCardEventHandles[MEMCARD_EVENT_HW_ERROR]);
            TestEvent(g_MemCardEventHandles[MEMCARD_EVENT_HW_TIMEOUT]);
            TestEvent(g_MemCardEventHandles[MEMCARD_EVENT_HW_NEW_CARD]);
            g_MemCardEventF400Spec2000Flag = 0;
            g_MemCardRemovedEventPending = 0;
            g_MemCardEventF400Spec0004Flag = 0;
            _card_load(port << 4);
            state->cardState = 3;
            return;
        }
        if (g_MemCardEventF000Spec8000Flag == 0 && g_MemCardEventF000Spec2000Flag == 0) {
            return;
        }
        g_MemCardEventF000Spec8000Flag = 0;
        g_MemCardEventF000Spec2000Flag = 0;
        state->cardState = 0;
        g_MemCardInfoPollCountdown = 0;
        g_MemCardServicedPort = g_MemCardServicedPort < 1;
        return;

    case 3:
        if (g_MemCardEventF400Spec0004Flag != 0) {
            g_MemCardEventF400Spec0004Flag = 0;
            state->cardState = 4;
            if (Menu_IsMemCardDialogOpen() != 0) {
                g_MemCardInfoPollCountdown = 0;
                g_MemCardServicedPort = g_MemCardServicedPort < 1;
                return;
            }
            state->present |= 1;
            MemCard_StartRead(port, 0);
            g_MemCardInfoPollCountdown = 0;
            g_MemCardServicedPort = g_MemCardServicedPort < 1;
            return;
        }
        if (g_MemCardRemovedEventPending != 0) {
            g_MemCardRemovedEventPending = 0;
            state->cardState = 0;
            g_MemCardInfoPollCountdown = 0;
            g_MemCardServicedPort = g_MemCardServicedPort < 1;
            return;
        }
        if (g_MemCardEventF400Spec2000Flag == 0) {
            return;
        }
        g_MemCardEventF400Spec2000Flag = 0;
        state->cardState = 4;
        state->present |= 4;
        g_MemCardInfoPollCountdown = 0;
        g_MemCardServicedPort = g_MemCardServicedPort < 1;
        return;

    case 4:
        state->present |= 1;
        break;

    default:
        return;
    }

    if (g_McOpPending != 0) {
        return;
    }
    if (port != g_MemCardServicedPort) {
        return;
    }

    value = g_MemCardInfoPollCountdown;
    g_MemCardInfoPollCountdown = value - 1;
    if (value > 0) {
        return;
    }

    TestEvent(g_MemCardEventHandles[MEMCARD_EVENT_HW_IO_END]);
    TestEvent(g_MemCardEventHandles[MEMCARD_EVENT_HW_ERROR]);
    TestEvent(g_MemCardEventHandles[MEMCARD_EVENT_HW_TIMEOUT]);
    TestEvent(g_MemCardEventHandles[MEMCARD_EVENT_HW_NEW_CARD]);
    g_MemCardEventF400Spec2000Flag = 0;
    g_MemCardRemovedEventPending = 0;
    g_MemCardEventF400Spec0004Flag = 0;
    _card_info(port << 4);
    state->cardState = 1;
}

void InitCARD(int arg0);
long StartCARD(void);
void _bu_init(void);
void _card_auto(int arg0);

int MemCard_OnEventF400Spec0004(void);
int MemCard_OnEventF400Spec8000(void);
int MemCard_OnEventF400Spec0100(void);
int MemCard_OnEventF400Spec2000(void);
int MemCard_OnEventF000Spec0004(void);
int MemCard_OnEventF000Spec8000(void);
int MemCard_OnEventF000Spec0100(void);
int MemCard_OnEventF000Spec2000(void);

void MemCard_InitManager(void) {
    int i;
    int *events;
    int value;
    int event;
    if (g_MemCardManagerReady == 0) {
        g_MemCardManagerReady = 1;
        i = 0;
        EnterCriticalSection();

        event = OpenEvent(0xF4000001, 0x0004, 0x1000, MemCard_OnEventF400Spec0004);
        events = g_MemCardEventHandles;
        events[0] = event;
        g_MemCardEventHandles[MEMCARD_EVENT_HW_ERROR] = OpenEvent(0xF4000001, 0x8000, 0x1000, MemCard_OnEventF400Spec8000);
        g_MemCardEventHandles[MEMCARD_EVENT_HW_TIMEOUT] = OpenEvent(0xF4000001, 0x0100, 0x1000, MemCard_OnEventF400Spec0100);
        g_MemCardEventHandles[MEMCARD_EVENT_HW_NEW_CARD] = OpenEvent(0xF4000001, 0x2000, 0x1000, MemCard_OnEventF400Spec2000);
        g_MemCardEventHandles[MEMCARD_EVENT_SW_IO_END] = OpenEvent(0xF0000011, 0x0004, 0x1000, MemCard_OnEventF000Spec0004);
        g_MemCardEventHandles[MEMCARD_EVENT_SW_ERROR] = OpenEvent(0xF0000011, 0x8000, 0x1000, MemCard_OnEventF000Spec8000);
        g_MemCardEventHandles[MEMCARD_EVENT_SW_TIMEOUT] = OpenEvent(0xF0000011, 0x0100, 0x1000, MemCard_OnEventF000Spec0100);
        g_MemCardEventHandles[MEMCARD_EVENT_SW_NEW_CARD] = OpenEvent(0xF0000011, 0x2000, 0x1000, MemCard_OnEventF000Spec2000);

        InitCARD(0);
        StartCARD();
        _bu_init();
        _card_auto(0);

        do {
            EnableEvent(events[i]);
            i++;
        } while (i < 8);

        ExitCriticalSection();
    }

    /* Clear each port's present flag, walking byte offsets downwards. */
    value = sizeof(MemCardPortState);
    do {
        ((u8 *)g_MemCardPortStates)[value] = 0;
        value -= sizeof(MemCardPortState);
    } while (value >= 0);
}

const MemCardSaveSupportData g_MemCardSaveSupportData __attribute__((used)) = {
    0x4353,
    0,
    "bu%ld0:%s",
    "bu 0:*",
    "bu%ld0:",
    {0, 0, 0, 0},
};

int Save_BuildCardFile(char *path) {
    u8 *resolved;
    MemCardFileHeader *file;
    u32 crc;

resolved = Text_GetSaveIconData();
    file = &g_MemCardIconHeader;

    g_MemCardSaveFileSizeWriteOnly = 0x2000;
    bzero(file, sizeof(*file));

    file->productCode = g_MemCardSaveSupportData.cardProductCode;
    file->iconDisplay = 0x11;
    file->blockCount = 1;
    strcpy(file->title, (char *)Save_FormatTitle(path[0x18] - 0x40, g_SaveRuntimeState.windows.header.playTimeMinutes));

    __builtin_memcpy(g_MemCardIconPalette, resolved + 0x14, 0x20);
    __builtin_memcpy(file->icon, resolved + 0x40, sizeof(file->icon));

    bzero(g_MemCardFileBuffer, 0x2000);
    g_SaveIoCursor = g_MemCardFileBuffer;
    __builtin_memcpy(g_SaveIoCursor, &g_MemCardIconHeader, sizeof(g_MemCardIconHeader));
    g_SaveIoCursor += 0x100;
    __builtin_memcpy(g_SaveIoCursor, (u8 *)&g_SaveRuntimeState, 0x12E4);
    g_SaveIoCursor += 0x12E4;

    Save_SerializeTail();

    {
        u8 *data = g_MemCardFileBuffer;
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
        name_buf = g_MemCardFilePath;
        /* Scheduling dependency for the retail argument setup. */
        asm("" : "=r"(name_buf) : "0"(name_buf), "r"(is_second));
        retry = 0;
        slot = state->selectedSlot;

        sprintf(name_buf, g_MemCardFilenameFormatPtr, is_second,
                        state->slots[slot].titleStyleFlag + 0x30, slot + 0x41);
        sprintf(path, g_MemCardSaveSupportData.saveFileFormat, state_index, g_MemCardFilePath);

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
    g_McOpPending = 0;
    MenuWidget_RestoreSavedCurrentNode();
}

static inline void MemCard_FormatSlotFileName(MemCardPortState *state,
                                              int slot) {
    sprintf(g_MemCardFilePath, g_MemCardFilenameFormatPtr, state > g_MemCardPortStates,
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

    if (strncmp(entry->name, &g_MemCardFilenameFormatPtr[6], 12) == 0) {
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
                g_McOpPending = 1;
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
                    g_McOpPending = 1;
                    state->managerState = 14;
                }
            }
        } else {
            g_MemCardSavePollTimeout = -1;
            state->managerState = 0;
        }
        break;

    case 14:
        sprintf(device, g_MemCardSaveSupportData.deviceFormat, port);
        g_MemCardSavePollTimeout = format(device) ? 12 : -1;
        state->present &= ~4;
        g_McOpPending = 0;
        state->managerState = 0;
        break;

    case 2:
        if (state->present != 1) {
            MemCard_AbortActiveOperation(state);
            break;
        }
        g_MemCardSearchPatternPtr[2] = port + '0';
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
        if (firstfile(g_MemCardSearchPatternPtr, &entry) != 0) {
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
        for (i = 0; i < 15; i++) {
            if (state->slots[i].state == MEMCARD_SLOT_EMPTY) {
                state->slots[i].metadataReady = 1;
            }
        }
        state->fileCount = 15;
        if (g_MemCardReadContext == 0) {
            state->managerState = 15;
            g_McOpPending = 0;
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
        g_McOpPending = 0;
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
            if ((state->fileDescriptor = open(g_MemCardFilePath, 1)) >= 0) {
                state->managerState = 8;
                state->transferData = g_MemCardPreviewBuffers[port].bytes;
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
            g_McOpPending = 0;
        }
        break;

    case 4:
        if (state->present != 1) {
            MemCard_AbortActiveOperation(state);
            break;
        }
        state->slots[state->selectedSlot].titleStyleFlag = g_SaveTitleStyleFlag;
        MemCard_FormatSlotFileName(state, state->selectedSlot);
        if ((fd = state->fileDescriptor = open(g_MemCardFilePath, 0x10200)) >= 0) {
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
        if ((state->fileDescriptor = open(g_MemCardFilePath, 1)) >= 0) {
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
        if ((state->fileDescriptor = open(g_MemCardFilePath, 2)) >= 0) {
            state->managerState = 9;
            state->transferData = g_MemCardFileBuffer;
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
                g_MemCardActiveState = 0;
                g_McOpPending = 0;
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
                g_MemCardActiveState = 0;
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
                                 &g_MemCardPreviewBuffers[port].preview);
        state->scanCursor++;
        state->managerState =
            state->scanCursor < state->fileCount * 2 ? 3 : 12;
        if (state->managerState == 12) {
            g_McOpPending = 0;
        }
        break;

    case 11:
        if (state->present != 1) {
            MemCard_AbortActiveOperation(state);
            break;
        }
        MemCard_FormatSlotFileName(state, state->selectedSlot);
        if ((fd = state->fileDescriptor = open(g_MemCardFilePath, 1)) >= 0) {
            close(fd);
            state->fileDescriptor = -1;
            MemCard_FormatSlotFileName(state, state->selectedSlot);
            if (erase(g_MemCardFilePath) != 0) {
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
    int slot;
    u8 formattedSlot;
    int status;
    int sequence;
    int next_state;
    char *path;
    char *formatBuffer;
    int firstSlotLetter;

    slot = arg_slot;
    state = &g_MemCardPortStates[port];
    status = state->present;
    if (status != 1) {
        return;
    }

    slot_state = &state->slots[slot];
    firstSlotLetter = 0x41;
    MemCard_CloseAll();
    formatBuffer = g_MemCardFilePath;
    Save_BuildHeader();

    sequence = state->sequence;
    {
        int next_sequence;

        if (slot_state->state == MEMCARD_SLOT_OCCUPIED) {
            next_sequence = sequence;
        } else {
            next_sequence = sequence + 1;
        }
        path = formatBuffer;
        state->selectedSlot = slot;
        state->sequence = next_sequence;
    }

    formattedSlot = (u8)slot;
    sprintf(path, g_MemCardFilenameFormatPtr,
                    state > g_MemCardPortStates,
                    state->slots[formattedSlot].titleStyleFlag + 0x30,
                    formattedSlot + firstSlotLetter);
    /* Recompute the path address after formatting it. */
    path = g_MemCardFilePath;
    Save_BuildCardFile(path);

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
    buffer = g_MemCardFileBuffer;
    state->transferData = buffer;
    state->transferSize = 0x2000;
    state->retryCount = 10;
    state->pendingError = 2;
    state->managerState = 1;
    state->nextState = 5;
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

void Menu_CreateNotificationDialog(int arg0, int arg1);
void Menu_SetDeferredCallback(void (*callback)(void));

void Save_LoadCardFileIntoRuntime(void) {
    SaveBytes4 expected_crc;
    u8 *data;
    u32 crc;
    u32 i;
    u32 bit;
    u8 *cursor;

    g_SaveIoCursor = (u8 *)&g_MemCardSaveStateBuffer;
    *(SaveBytes12E4 *)&g_SaveRuntimeState = g_MemCardSaveStateBuffer;
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

int MemCard_GetActiveProgressBlocks(void) {
    int blocks;
    int result;
    MemCardPortState *active = g_MemCardActiveState;

    if (active != 0) {
        blocks = (g_MemCardActiveBytesRemaining - active->transferSize + 0x400) >> 10;
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

MemCardSaveSlot *MemCard_GetSlot(unsigned int port, int slot) {
    if (port >= 2U) {
        return 0;
    }
    if (slot < 0) {
        return 0;
    }
    if (slot >= g_MemCardPortStates[port].fileCount) {
        return 0;
    }
    if (g_MemCardPortStates[port].slots[slot].metadataReady == 0) {
        return 0;
    }
    return &g_MemCardPortStates[port].slots[slot];
}

void MemCard_InitState(void) {
    bzero(g_MemCardPortStates, sizeof(MemCardPortState) * 2);
    g_MemCardPortStates[1].fileDescriptor = -1;
    g_MemCardPortStates[0].fileDescriptor = -1;
    g_MemCardServicedPort = 0;
    g_MemCardInfoPollCountdown = 0;
    g_MemCardConnectDebounce = 0;
    g_MemCardConnectPending[1] = 0;
    g_MemCardConnectPending[0] = 0;
    g_McOpPending = 0;
    g_MemCardActiveState = 0;
    g_MemCardActivePortOneBased = 0;
    g_MemCardSavePollTimeout = 0;
    g_MemCardActivePromptPending = 0;
    g_SaveTitleStyleFlag = 0;
    g_MemCardLoadSucceeded = 0;
    g_MemCardReadContext = 0;
}

void MemCard_StepPortState(int arg0);
void Menu_CloseNotificationDialogs(void);
void Menu_NavToSaveConfirmDialog(void);
void MemCard_ClearActivePrompt(void);
void MemCard_StartActivePortRead(void);

void MemCard_UpdateSavePolling(void)
{
    int port;
    MemCardPortState *state;
    int timeout;
    int activeState;

    MemCard_StepPortState(1);
    MemCard_StepPortState(0);
    if (g_MemCardSavePollTimeout != 0) {
        activeState = g_MemCardPortStates[g_MemCardActivePortOneBased - 1].cardState;
        if (activeState != 4 && activeState != 1) {
            g_MemCardSavePollTimeout = -2;
        }
        timeout = g_MemCardSavePollTimeout;
        timeout -= timeout > 0;
        g_MemCardSavePollTimeout = timeout;
        if (timeout <= 0) {
            Menu_DestroyMemCardProgressWidget();
            switch (g_MemCardSavePollTimeout) {
            case 0:
                /* Timed out without an error: offer to read the card again. */
                Menu_CreateNotificationDialog(0x52, 0);
                Menu_SetDeferredCallback(MemCard_StartActivePortRead);
                g_MemCardActivePromptPending = 1;
                break;
            case -1:
                Menu_CreateNotificationDialog(0x3C, 0);
                Menu_SetDeferredCallback(MemCard_ClearActivePrompt);
                g_MemCardActivePromptPending = 1;
                break;
            default:
                MemCard_ClearActivePrompt();
                break;
            }
            g_MemCardSavePollTimeout = 0;
        }
    }

    port = 1;
    state = &g_MemCardPortStates[1];
    do {
        if (g_MemCardActivePortOneBased == port + 1 && !(state->present & 1)) {
            Menu_NavToSaveConfirmDialog();
            Menu_CloseNotificationDialogs();
            MemCard_ClearActivePrompt();
        }
        MemCard_UpdateSaveState(port);
        port--;
        state--;
    } while (port >= 0);
}

int MemCard_IsPortPresent(int port) {
    return g_MemCardPortStates[port].present & 1;
}

void MemCard_CloseAll(void) {
    MemCardPortState *state;

    for (state = g_MemCardPortStates; state < &g_MemCardPortStates[2]; state++) {
        if (state->managerState == 8 || state->managerState == 10) {
            close(state->fileDescriptor);
            state->fileDescriptor = -1;
            state->managerState = 12;
        }
    }
}

void Menu_StepItemGrid2(void);

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

void MemCard_MarkActivePortState13(void) {
    int portIndex = g_MemCardActivePortOneBased - 1;
    g_MemCardPortStates[portIndex].managerState = 0xD;
}

void MemCard_ClearActivePrompt(void) {
    g_MemCardActivePortOneBased = 0;
    g_MemCardActivePromptPending = 0;
}

void MenuWidget_SaveAndSetCurrentNode(MenuWidgetNode *arg0);

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

void MemCard_CloseAllAndResetState(void) {
    MemCardPortState *state;

    for (state = g_MemCardPortStates; state < &g_MemCardPortStates[2]; state++) {
        if (state->managerState == 8 || state->managerState == 10) {
            close(state->fileDescriptor);
            state->fileDescriptor = -1;
            state->managerState = 12;
        }
    }

    g_MemCardPortStates[1].managerState = 0;
    g_MemCardPortStates[0].managerState = 0;
    g_McOpPending = 0;
}

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
