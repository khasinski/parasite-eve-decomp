#ifndef PE1_PSYQ_PAD_MAIN_H
#define PE1_PSYQ_PAD_MAIN_H

#include "pe1/card_obj.h"
#include "pe1/memcard_state.h"

/* BIOS interrupt queue entry for SysEnqIntRP/SysDeqIntRP: the link word,
 * the two handlers the BIOS calls and a spare word, all cleared or set by
 * the code that enqueues it. */
typedef struct PadIntQueueNode {
    void *next;
    void *handlers[2];
    int spare;
} PadIntQueueNode;

PE1_STATIC_ASSERT(sizeof(PadIntQueueNode) == 0x10, pad_int_queue_node_size);

/* LIBPAD/PADMAIN.OBJ. Legacy MemCard/Spu names are preserved for callers. */
extern int g_MemCardServiceReady, g_MemCardCommandStep;
extern PadIntQueueNode g_PadVsyncIntQueueNode;
extern MemCardInterruptRegisters *D_8009B784;
extern MemCardInterruptRegisters *D_8009B24C;
extern MemCardSioRegisters *D_8009B788;
extern MemCardSioRegisters *D_8009B250;
/* Per-port idle counters that PadEnableCom and the VSync timer age. */
extern int g_PadPortIdleFrames[2];
extern int (*g_MemCardCommandStepHandlers[])(void);
extern int (*g_PadDmaCompleteFn)(int);
extern u32 g_TimerTimeoutLimit, g_TimerTimeoutStart;
void setRC2wait(int);
int MemCard_WaitReadyForTransfer(void);
int chkRC2wait(void);
void _padStartCom(void);
void MemCard_RunCommandStep(CardObj *port);
int _padSioRW(CardObj *, int);

/* Pad interrupt queue entry removed by StopPAD. */
extern int g_PadIntQueueNode;

#endif
