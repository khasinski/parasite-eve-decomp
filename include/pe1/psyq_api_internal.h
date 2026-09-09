#ifndef PE1_PSYQ_API_INTERNAL_H
#define PE1_PSYQ_API_INTERNAL_H

#include "common.h"
#include "pe1/psyq_callbacks.h"
typedef void (*PadToggleFunc)(void);

typedef struct RootCounter {
    u16 rootCounter;
    s16 field_02;
    s16 mode;
    s16 field_06;
    s16 target;
    s16 field_0A;
    s32 field_0C;
} RootCounter;

typedef struct InterruptControlRegisters {
    s32 status;
    s32 mask;
} InterruptControlRegisters;

extern RootCounter *D_8009B7D0;
extern InterruptControlRegisters *D_8009B7CC;
extern s32 D_8009B7D4[4];
extern PadToggleFunc jtbl_800A34C8;
extern PadToggleFunc jtbl_800A34CC;

#define _counters ((volatile RootCounter *)D_8009B7D0)
#define _interrupt_status_register ((s32 *)D_8009B7CC)
#define _interrupt_status_masks D_8009B7D4

typedef PsyqInterruptHandler DmaInterruptCallback;
typedef DmaInterruptCallback (*DmaCallbackSetter)(int channel,
                                               DmaInterruptCallback callback);
extern DmaInterruptCallback g_IntrDmaHandlerTable[];
/* Initialized to the DMA interrupt control register at 0x1F8010F4. */
extern unsigned int *g_IntrDmaDispatchPtr;
DmaInterruptCallback setIntrDMA(int channel, DmaInterruptCallback callback);
DmaCallbackSetter startIntrDMA(void);
void memclrIntrDMA(int *ptr, int count);
void trapIntrDMA(void);

typedef PsyqInterruptHandler (*VSyncCallbackSetter)(unsigned int index,
                                                  PsyqInterruptHandler callback);
typedef struct InterruptDispatchTable {
    unsigned int unknown_00;
    DmaCallbackSetter dma;
    DmaCallbackSetter interrupt;
    int (*reset)(void);
    int (*stop)(void);
    VSyncCallbackSetter vsync;
    int (*restart)(void);
} InterruptDispatchTable;

PE1_STATIC_ASSERT(PE1_OFFSETOF(InterruptDispatchTable, dma) == 4, intr_dispatch_dma);
PE1_STATIC_ASSERT(PE1_OFFSETOF(InterruptDispatchTable, interrupt) == 8, intr_dispatch_irq);
PE1_STATIC_ASSERT(PE1_OFFSETOF(InterruptDispatchTable, vsync) == 20, intr_dispatch_vsync);
PE1_STATIC_ASSERT(sizeof(InterruptDispatchTable) == 28, intr_dispatch_size);

extern InterruptDispatchTable *g_EventCallbackTable;
extern PsyqInterruptHandler g_IntrVSyncCallbackTable[];
extern volatile int g_VSyncCount;
void trapIntrVSync(void);
VSyncCallbackSetter startIntrVSync(void);
PsyqInterruptHandler setIntrVSync(unsigned int index, PsyqInterruptHandler callback);

#endif
