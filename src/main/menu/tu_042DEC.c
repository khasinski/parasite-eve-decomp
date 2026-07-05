extern void * volatile g_AkaoBgmHandle;
extern void Akao_SendTableCommand(void *, int, int, int, int);

void Menu_PlayConfirmSound(void) {
    void * volatile *slot;

    slot = &g_AkaoBgmHandle;
    if (*slot != 0) {
        Akao_SendTableCommand(*slot, 0x44C, 0x100, 0x80, 0x7F);
    }
}

void Menu_PlayCancelSound(void) {
    void * volatile *slot;

    slot = &g_AkaoBgmHandle;
    if (*slot != 0) {
        Akao_SendTableCommand(*slot, 0x44D, 0x100, 0x80, 0x7F);
    }
}

void Menu_PlayMoveSound(void) {
    void * volatile *slot;

    slot = &g_AkaoBgmHandle;
    if (*slot != 0) {
        Akao_SendTableCommand(*slot, 0x44E, 0x100, 0x80, 0x7F);
    }
}

void Menu_PlayErrorSound(void) {
    void * volatile *slot;

    slot = &g_AkaoBgmHandle;
    if (*slot != 0) {
        Akao_SendTableCommand(*slot, 0x44F, 0x100, 0x80, 0x7F);
    }
}
