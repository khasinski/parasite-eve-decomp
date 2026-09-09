#include "common.h"

typedef struct BiosC0Table {
    u32 *entries[6];
    u32 *exception_handler;
} BiosC0Table;
typedef BiosC0Table *(*GetC0TableService)(void);
PE1_STATIC_ASSERT(PE1_OFFSETOF(BiosC0Table, exception_handler) == 0x18, c0_exception_entry);

extern void *D_800A3450;
/* Two six-word instruction templates, not callable function bodies. */
extern const u32 St_DmaCompleteCallback[6];
extern const u32 D_8007A1F8[6];
int EnterCriticalSection(void);
void ExitCriticalSection(void);
void FlushCache(void);

void St_InstallDmaHandler(void) {
    register void *caller asm("$31");
    register u32 selector asm("$9");
    BiosC0Table *table;
    u32 *handler;
    u32 *cursor;
    const u32 *source;
    asm volatile("" : "=r"(caller) : "0"(caller));
    D_800A3450 = caller;
    EnterCriticalSection();
    selector = 0x56;
    asm volatile("" : : "r"(selector));
    table = ((GetC0TableService)0xB0)();
    handler = table->exception_handler + 10;
    cursor = handler;
    source = St_DmaCompleteCallback;
    do {
        if (*cursor++ != *source++) {
            goto done;
        }
    } while (source != St_DmaCompleteCallback + 6);
    cursor = handler;
    source = D_8007A1F8;
    do {
        *cursor++ = *source++;
    } while (source != D_8007A1F8 + 6);
done:
    FlushCache();
    ExitCriticalSection();
}
