/* ASSEMBLER: GNU */

/* CC1_FLAGS: -fno-schedule-insns */

extern void func_80084B78(void);
extern void CardObj_IsTransferActive(void);
extern void LIBPAD_PADSEQD_text_108(void);

extern void (*g_MemCardStateDispatchFn)(void);
extern void (*g_MemCardIsTransferActiveFn)(void);

extern void (*g_MemCardResponseHandler)(void);

void MemCard_SetCallbackVtable(void) {
    void (*response_handler)(void);

    g_MemCardStateDispatchFn = func_80084B78;
    g_MemCardIsTransferActiveFn = CardObj_IsTransferActive;
    response_handler = LIBPAD_PADSEQD_text_108;
    g_MemCardResponseHandler = response_handler;
}
