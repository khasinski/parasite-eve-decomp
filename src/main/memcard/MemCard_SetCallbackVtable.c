/* MASPSX_FLAGS: --store-return-delay */

extern void func_80084B78(void);
extern void CardObj_IsTransferActive(void);
extern void CardObj_HandleResponse(void);

extern void (*g_MemCardStateDispatchFn)(void);
extern void (*g_MemCardIsTransferActiveFn)(void);
extern void (*g_MemCardResponseHandler)(void);

void MemCard_SetCallbackVtable(void) {
    g_MemCardStateDispatchFn = func_80084B78;
    g_MemCardIsTransferActiveFn = CardObj_IsTransferActive;
    g_MemCardResponseHandler = CardObj_HandleResponse;
}
