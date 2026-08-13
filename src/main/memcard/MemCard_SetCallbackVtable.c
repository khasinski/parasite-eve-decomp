
/* CC1_FLAGS: -fno-schedule-insns */

extern void func_80084B78(void);
extern void CardObj_IsTransferActive(void);
extern void CardObj_HandleResponse(void);

typedef struct MemCardCallbackDataPage {
    void (*response_handler)(void);
    char reserved04[0x48B8];
} MemCardCallbackDataPage;

extern void (*g_MemCardStateDispatchFn)(void);
extern void (*g_MemCardIsTransferActiveFn)(void);

register MemCardCallbackDataPage *g_MemCardCallbackWritePage asm("$1");

void MemCard_SetCallbackVtable(void) {
    void (*response_handler)(void);

    g_MemCardStateDispatchFn = func_80084B78;
    g_MemCardIsTransferActiveFn = CardObj_IsTransferActive;
    response_handler = CardObj_HandleResponse;
    g_MemCardCallbackWritePage = (MemCardCallbackDataPage *)0x800A0000;
    g_MemCardCallbackWritePage[-1].response_handler = response_handler;
}
