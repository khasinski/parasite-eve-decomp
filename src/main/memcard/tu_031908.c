typedef signed short s16;
typedef unsigned char u8;
typedef unsigned short u16;

typedef struct SaveSlotRaw {
    u8 active;
    u8 field_01;
    u8 pad_02[2];
    int field_04;
    int field_08;
    int field_0C;
    int field_10;
    int field_14;
    int field_18;
    int field_1C;
    int field_20;
    u16 field_24;
    u16 field_26;
    u8 field_28;
    u8 name_digit;
    u8 field_2A;
    u8 pad_2B;
    u16 field_2C;
    u16 field_2E;
    u8 pad_30[0x14];
} SaveSlotRaw;

typedef struct SavePortRaw {
    u8 status;
    u8 state;
    u8 field_02;
    u8 selected_slot;
    u8 has_files;
    u8 list_cursor;
    u8 scan_cursor;
    u8 pending_error;
    u8 pad_08[2];
    u8 sequence;
    u8 next_state;
    int fd;
    u16 transfer_size;
    u16 retry_count;
    u8 *transfer_ptr;
    SaveSlotRaw slots[15];
} SavePortRaw;

extern char D_800A0ED4[];
extern char D_8009EE70[];
extern char *D_80092224;
extern void *D_80092230;
extern void *D_800A1854;
extern int D_800A1838;
extern int D_800A1858;
extern int D_800A1864;
extern int D_800A186C;
extern int D_800A1704;
extern u8 D_800A1720[];
extern u8 D_8009EED0[];

void MemCard_CloseAll(void);
void MemCard_AbortActiveOperation(void *state);
int MemCard_GetDialogMode(void);
void MenuWidget_RestoreSavedCurrentNode(void);
int Menu_CreateSaveSlotListView(int port, int selected);
void Menu_ShowMemCardErrorDialog(int port);
void Menu_DestroyMemCardProgressWidget(void);
void Menu_CreateNotificationDialog(int text_id, int arg1);
void Menu_CreateTwoLineDialog(int text_id_a, int text_id_b);
void Save_BuildHeader(void);
int Save_BuildCardFile(char *path);
void Save_LoadCardFileIntoRuntime(void);
int Scene_CreateEntityNode(void *arg0, void *arg1);
int Square_Vsprintf();
int close(int fd);
int erase(char *path);
int format(char *path);
int lseek(int fd, int offset, int whence);
int nextfile(void *arg0);
int open(char *path, int mode);
int read(int fd, void *buf, int size);
int strncmp(char *a, char *b, int len);
int write(int fd, void *buf, int size);

static SavePortRaw *MemCard_PortState(int port) {
    return (SavePortRaw *)(D_800A0ED4 + (((port << 5) + port) * 4 - port) * 8);
}

static SavePortRaw *MemCard_OtherPortState(int port) {
    return (SavePortRaw *)(D_800A0ED4 + (port == 0 ? 0x418 : 0));
}

static int MemCard_PortIndex(SavePortRaw *state) {
    return state > (SavePortRaw *)D_800A0ED4;
}

static SaveSlotRaw *MemCard_Slot(SavePortRaw *state, int slot) {
    return &state->slots[slot];
}

static void MemCard_BuildPath(SavePortRaw *state, int slot) {
    SaveSlotRaw *entry;

    entry = MemCard_Slot(state, slot);
    Square_Vsprintf(D_8009EE70, D_80092224, MemCard_PortIndex(state), entry->name_digit + 0x30, slot + 0x41);
}

static int MemCard_WaitOrAbort(SavePortRaw *state, int one_error, int two_error, int fallback_state) {
    int retries;

    retries = state->retry_count;
    state->retry_count = retries - 1;
    if ((retries << 16) > 0) {
        return 0;
    }

    if ((state->status & 1) == 0) {
        MemCard_AbortActiveOperation(state);
        return -1;
    }

    MemCard_AbortActiveOperation(state);
    if (state->pending_error == 1) {
        Menu_CreateTwoLineDialog(0x3D, 0x3F);
        state->pending_error = 0;
        state->state = 0x0C;
        return 1;
    }
    if (state->pending_error == 2) {
        Menu_CreateNotificationDialog(two_error, 0);
        state->pending_error = 0;
        state->state = 0x0C;
        return 1;
    }

    state->pending_error = 0;
    state->state = fallback_state;
    return 1;
}

static void MemCard_ResetAndAbort(SavePortRaw *state) {
    if (state->status != 1) {
        MemCard_AbortActiveOperation(state);
    }
}

static void MemCard_ClearGlobalBusy(SavePortRaw *state, int new_state) {
    state->state = new_state;
    D_800A1838 = 0;
}

static void MemCard_InitSlotScan(SavePortRaw *state, int port) {
    u8 *node;
    int i;
    int found;
    int blocks;
    int slot;
    SaveSlotRaw *entry;

    ((u8 *)D_80092230)[2] = port + 0x30;
    state->field_02 = 0;
    state->selected_slot = 0;
    state->scan_cursor = 0;
    state->has_files = 0;
    state->pending_error = 0;
    state->sequence = 0;

    for (i = 0; i < 15; i++) {
        state->slots[i].active = 2;
    }

    found = 0;
    node = (u8 *)D_80092230;
    if (Scene_CreateEntityNode(D_80092230, node) != 0) {
        do {
            if (strncmp((char *)node, D_80092224 + 6, 0x0C) == 0) {
                slot = node[0x33];
                entry = MemCard_Slot(state, slot);
                entry->active = 1;
                entry->field_01 = 0;
                entry->name_digit = node[0x32] != 0x30;
                state->has_files = 1;
                found = 1;
            }
            blocks = (*(int *)(node + 0x38)) >> 13;
            state->sequence += blocks;
        } while (nextfile(node) != 0);
        state->retry_count = 0;
    } else {
        state->retry_count--;
    }

    if ((s16)state->retry_count > 0) {
        return;
    }

    for (i = 0; i < 15; i++) {
        if (state->slots[i].active == 2) {
            state->slots[i].field_01 = 1;
        } else {
            state->sequence--;
        }
    }

    if (state->sequence != 0) {
        for (i = 14; i >= 0 && state->sequence != 0; i--) {
            if (state->slots[i].active == 2) {
                state->slots[i].active = 3;
                state->sequence--;
            }
        }
    }

    state->field_02 = 15;
    if (D_800A186C == 0) {
        state->state = 15;
        D_800A1838 = 0;
        return;
    }

    found = 0;
    for (i = 0; i < 15; i++) {
        if (state->slots[i].active == 1) {
            found = 1;
            break;
        }
    }

    if (found || (state->sequence < 15 && MemCard_GetDialogMode() != 0)) {
        state->state = 3;
        state->retry_count = 10;
        MenuWidget_RestoreSavedCurrentNode();
        state->list_cursor = Menu_CreateSaveSlotListView(port, state->field_02);
    } else {
        MenuWidget_RestoreSavedCurrentNode();
        Menu_ShowMemCardErrorDialog(port);
        MemCard_ClearGlobalBusy(state, 0x0C);
    }
}

static int MemCard_FindNextSlot(SavePortRaw *state) {
    int cursor;
    int count;
    int delta;
    int slot;

    cursor = state->scan_cursor;
    count = state->field_02 * 2;
    while (cursor < count) {
        delta = ((cursor & 1) << 1) - 1;
        slot = state->list_cursor + delta * ((cursor + 1) >> 1);
        if ((unsigned int)slot < 15 && state->slots[slot].field_01 == 0) {
            state->scan_cursor = cursor;
            state->selected_slot = slot;
            return slot;
        }
        cursor++;
        state->scan_cursor = cursor;
    }

    state->selected_slot = 0xFF;
    return -1;
}

static void MemCard_StartReadHeader(SavePortRaw *state, int port) {
    int slot;
    int fd;

    if (state->status != 1) {
        MemCard_ResetAndAbort(state);
        return;
    }

    slot = MemCard_FindNextSlot(state);
    if (slot < 0) {
        MemCard_ClearGlobalBusy(state, 0x0C);
        return;
    }

    MemCard_BuildPath(state, slot);
    fd = open(D_8009EE70, 1);
    state->fd = fd;
    if (fd >= 0) {
        state->state = 8;
        state->transfer_ptr = D_800A1720 + (port << 7);
        state->transfer_size = 0x80;
        state->retry_count = 0x1E;
        if (state->slots[slot].active == 1) {
            lseek(fd, 0x100, 0);
        }
        return;
    }

    if (MemCard_WaitOrAbort(state, 0x3D, 0x3E, 0x0C) < 0) {
        return;
    }
}

static void MemCard_StartCreate(SavePortRaw *state) {
    int fd;

    if (state->status != 1) {
        MemCard_ResetAndAbort(state);
        return;
    }

    D_800A1704 = state->slots[state->selected_slot].name_digit;
    MemCard_BuildPath(state, state->selected_slot);
    fd = open(D_8009EE70, 0x10200);
    state->fd = fd;
    if (fd >= 0) {
        close(fd);
        state->fd = -1;
        state->state = 6;
        return;
    }

    MemCard_WaitOrAbort(state, 0x3D, 0x3E, 0x0C);
}

static void MemCard_StartOpenForRead(SavePortRaw *state) {
    int fd;

    if (state->status != 1) {
        MemCard_ResetAndAbort(state);
        return;
    }

    MemCard_BuildPath(state, state->selected_slot);
    fd = open(D_8009EE70, 1);
    state->fd = fd;
    if (fd >= 0) {
        state->retry_count = 0x1E;
        state->state = 7;
        return;
    }

    MemCard_WaitOrAbort(state, 0x3D, 0x3E, 0x0C);
}

static void MemCard_StartOpenForWrite(SavePortRaw *state) {
    int fd;

    if (state->status != 1) {
        MemCard_ResetAndAbort(state);
        return;
    }

    MemCard_BuildPath(state, state->selected_slot);
    fd = open(D_8009EE70, 2);
    state->fd = fd;
    if (fd >= 0) {
        state->state = 9;
        state->transfer_ptr = D_8009EED0;
        state->retry_count = 0x1E;
        return;
    }

    MemCard_WaitOrAbort(state, 0x3D, 0x3E, 0x0C);
}

static void MemCard_ReadChunk(SavePortRaw *state, int chunk_size, int done_state) {
    int size;
    int got;

    if (state->status != 1) {
        MemCard_ResetAndAbort(state);
        return;
    }

    size = state->transfer_size;
    if (size > chunk_size) {
        size = chunk_size;
    }

    got = read(state->fd, state->transfer_ptr, size);
    if (got > 0) {
        state->transfer_ptr += got;
        state->transfer_size -= got;
        if ((s16)state->transfer_size <= 0) {
            state->state = done_state;
        }
        return;
    }

    MemCard_WaitOrAbort(state, 0x3D, 0x3E, 0x0C);
}

static void MemCard_WriteChunk(SavePortRaw *state) {
    int size;
    int wrote;

    if (state->status != 1) {
        MemCard_ResetAndAbort(state);
        return;
    }

    size = state->transfer_size;
    if (size > 0x400) {
        size = 0x400;
    }

    wrote = write(state->fd, state->transfer_ptr, size);
    if (wrote > 0) {
        state->transfer_ptr += wrote;
        state->transfer_size -= wrote;
        if ((s16)state->transfer_size <= 0) {
            close(state->fd);
            state->fd = -1;
            state->state = 3;
            state->pending_error = 0;
            D_800A1854 = 0;
            Menu_DestroyMemCardProgressWidget();
            Menu_CreateNotificationDialog(0x53, 0);
        }
        return;
    }

    MemCard_WaitOrAbort(state, 0x3D, 0x3E, 0x0C);
}

static void MemCard_FinalizeReadSlot(SavePortRaw *state, int port) {
    SaveSlotRaw *entry;
    u8 *card;

    if (state->status != 1) {
        MemCard_ResetAndAbort(state);
        return;
    }

    close(state->fd);
    state->fd = -1;
    entry = &state->slots[state->selected_slot];
    card = D_800A1720 + (port << 7);

    if (entry->active == 1) {
        entry->field_24 = *(u16 *)(card + 0x28);
        entry->field_26 = *(u16 *)(card + 0x26);
        entry->field_28 = card[0x2A];
        entry->field_0C = *(int *)(card + 8);
        entry->field_10 = *(int *)(card + 0x0C);
        entry->field_20 = *(int *)(card + 0x64);
        entry->field_2A = card[0x2B];
        entry->field_2C = *(u16 *)(card + 0x5C);
        entry->field_2E = *(u16 *)(card + 0x5E);
        entry->field_04 = *(int *)(card + 0);
        entry->field_08 = *(int *)(card + 4);
        entry->field_14 = *(int *)(card + 0x10);
        entry->field_18 = *(int *)(card + 0x14);
        entry->field_1C = *(int *)(card + 0x18);
    }

    entry->field_01 = 1;
    state->scan_cursor++;
    if (state->scan_cursor < state->field_02 * 2) {
        state->state = 3;
    } else {
        MemCard_ClearGlobalBusy(state, 0x0C);
    }
}

static void MemCard_EraseSelected(SavePortRaw *state) {
    int result;

    if (state->status != 1) {
        MemCard_ResetAndAbort(state);
        return;
    }

    MemCard_BuildPath(state, state->selected_slot);
    state->fd = open(D_8009EE70, 1);
    if (state->fd < 0) {
        MemCard_WaitOrAbort(state, 0x3D, 0x3E, 0x0C);
        return;
    }

    close(state->fd);
    state->fd = -1;
    MemCard_BuildPath(state, state->selected_slot);
    result = erase(D_8009EE70);
    if (result != 0) {
        state->state = 4;
        state->retry_count = 10;
        return;
    }

    MemCard_WaitOrAbort(state, 0x3D, 0x3E, 0x0C);
}

void Save_StartWriteSlot(int port, int slot) {
    SavePortRaw *state;
    SaveSlotRaw *slot_state;
    int seq;
    int next_state;

    state = MemCard_PortState(port);
    if (state->status == 1) {
        slot_state = &state->slots[slot];
        MemCard_CloseAll();
        Save_BuildHeader();

        seq = state->sequence;
        if (slot_state->active != 1) {
            seq++;
        }
        state->selected_slot = slot;
        state->sequence = seq;

        MemCard_BuildPath(state, slot);
        Save_BuildCardFile(D_8009EE70);

        state->list_cursor = slot;
        state->scan_cursor = 0;
        slot_state->field_01 = 0;
        state->transfer_size = 0x2000;
        state->retry_count = 10;
        state->state = 1;
        state->pending_error = 1;
        D_800A1854 = state;
        D_800A1858 = 0x2000;

        next_state = 4;
        if (slot_state->active == 1) {
            next_state = 0x0B;
        }
        state->next_state = next_state;
        slot_state->active = 1;
    }
}

void MemCard_UpdateSaveState(int port) {
    SavePortRaw *state;
    SavePortRaw *other;
    char path[64];

    state = MemCard_PortState(port);
    switch (state->state) {
    case 0:
        if (state->status == 1 && state->next_state == 4) {
            other = MemCard_OtherPortState(port);
            if (other->next_state == 0 || other->next_state == state->next_state) {
                D_800A1838 = state->status;
                state->state = state->next_state;
            }
        } else {
            MemCard_ResetAndAbort(state);
        }
        break;
    case 1:
        if (state->status == 5 && state->next_state == 4) {
            other = MemCard_OtherPortState(port);
            if (other->next_state == 0 || other->next_state == state->next_state) {
                D_800A1838 = 1;
                state->state = 0x0E;
            }
        } else {
            D_800A1864 = -1;
            state->state = 0;
        }
        break;
    case 2:
        Square_Vsprintf(path, "%s%d", port);
        D_800A1864 = format(path) != 0 ? 0x0C : -1;
        state->status &= ~4;
        D_800A1838 = 0;
        state->state = 0;
        break;
    case 3:
        MemCard_InitSlotScan(state, port);
        break;
    case 4:
        MemCard_StartReadHeader(state, port);
        break;
    case 5:
        MemCard_StartCreate(state);
        break;
    case 6:
        MemCard_StartOpenForRead(state);
        break;
    case 7:
        MemCard_ReadChunk(state, 0x80, 0x0A);
        break;
    case 8:
        MemCard_ReadChunk(state, 0x400, 0x0C);
        if (state->state == 0x0C) {
            close(state->fd);
            state->fd = -1;
            state->pending_error = 0;
            D_800A1854 = 0;
            D_800A1838 = 0;
            Save_LoadCardFileIntoRuntime();
        }
        break;
    case 9:
        MemCard_WriteChunk(state);
        break;
    case 10:
        MemCard_FinalizeReadSlot(state, port);
        break;
    case 11:
        MemCard_EraseSelected(state);
        break;
    case 12:
        if (state->status != 1) {
            MemCard_AbortActiveOperation(state);
        }
        break;
    case 14:
        if (state->status == 1) {
            D_800A1864 = 0;
            state->state = 2;
        } else {
            MemCard_AbortActiveOperation(state);
        }
        break;
    case 15:
        D_800A1838 = 0;
        break;
    default:
        MemCard_AbortActiveOperation(state);
        break;
    }
}
