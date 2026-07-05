/* MASPSX_FLAGS: --stack-return-delay */

extern int EnterCriticalSection(void);
extern void ExitCriticalSection(void);
extern void SysDeqIntRP(int index, void *queue);
extern void SysEnqIntRP(int index, void *queue);

typedef struct {
    void (*callback0)(void);
    void (*callback1)(void);
} CallbackPair;

extern int D_800A34B0;
extern CallbackPair D_800A34B4;
extern int D_800A34BC;

void Pad_VBlankHandler(void);
int Pad_CheckTransferDone(void);

int Sys_SetInterruptHandler(void) {
    register int one asm("$4");
    int *queue;
    CallbackPair *callbacks;

    EnterCriticalSection();
    one = 1;
    callbacks = &D_800A34B4;
    asm volatile("addiu\t$16,$3,-4" : "=r"(queue) : "r"(callbacks));
    callbacks->callback0 = Pad_VBlankHandler;
    asm volatile(
        "lui\t$2,%%hi(Pad_CheckTransferDone)\n\t"
        "addiu\t$2,$2,%%lo(Pad_CheckTransferDone)\n\t"
        "sw\t$2,4($3)"
        :
        : "r"(callbacks)
        : "$2", "memory");
    D_800A34B0 = 0;
    D_800A34BC = 0;
    SysDeqIntRP(one, queue);
    one = 1;
    SysEnqIntRP(one, queue);
    ExitCriticalSection();
    return 1;
}
