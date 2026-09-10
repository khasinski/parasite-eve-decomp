#ifndef PE1_PSYQ_API_INTERNAL_H
#define PE1_PSYQ_API_INTERNAL_H

#include "common.h"
#include "pe1/psyq_callbacks.h"
typedef void (*PadToggleFunc)(void);

/* LIBETC INTR.OBJ state: a 12-word BIOS jump buffer and 4 KiB IRQ stack. */
typedef struct InterruptCallbackState {
    u16 active;
    u16 inCallback;
    PsyqInterruptHandler handlers[11];
    u16 enabled;
    u16 interruptMask;
    u32 dmaControl;
    u32 entry[12];
    s32 interruptStack[1024];
} InterruptCallbackState;

PE1_STATIC_ASSERT(sizeof(InterruptCallbackState) == 0x1068, intr_state_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(InterruptCallbackState, handlers) == 4,
                  intr_state_handlers);
PE1_STATIC_ASSERT(PE1_OFFSETOF(InterruptCallbackState, enabled) == 0x30,
                  intr_state_enabled);
PE1_STATIC_ASSERT(PE1_OFFSETOF(InterruptCallbackState, interruptMask) == 0x32,
                  intr_state_saved_mask);
PE1_STATIC_ASSERT(PE1_OFFSETOF(InterruptCallbackState, dmaControl) == 0x34,
                  intr_state_saved_dma);
PE1_STATIC_ASSERT(PE1_OFFSETOF(InterruptCallbackState, entry) == 0x38,
                  intr_state_entry);
PE1_STATIC_ASSERT(PE1_OFFSETOF(InterruptCallbackState, interruptStack) == 0x68,
                  intr_state_stack);

/* Legacy halfword storage and the handler-array view into that state. */
extern u16 D_800945E4[];
extern PsyqInterruptHandler D_800945E8[11];
extern u16 *D_80095670;
extern volatile u16 *D_80095674;
extern u16 *g_IntrMaskRegPtr;
extern u32 *D_80095678;
PsyqInterruptHandler Sys_SetIntrCallback(int channel, PsyqInterruptHandler handler);
InterruptCallbackState *Sys_SaveDisableIntr(void);
InterruptCallbackState *Sys_RestoreIntr(void);


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

typedef struct DmaChannelRegisters {
    u32 address;
    u32 block_control;
    u32 channel_control;
    u32 unused;
} DmaChannelRegisters;
PE1_STATIC_ASSERT(sizeof(DmaChannelRegisters) == 16, dma_channel_stride);
extern DmaChannelRegisters *D_800956E0;
extern char D_8001177C[];
extern char D_80011798[];

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
