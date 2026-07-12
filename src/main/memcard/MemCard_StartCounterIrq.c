/* MASPSX_FLAGS: --store-call-delay --stack-return-delay */

extern int D_8009B75C;
extern void *D_800A5AB0;
extern int *D_8009B784;
extern int D_8009B758;
extern void (*D_8009B728)(int);
extern char D_800A5AC0[];

void EnterCriticalSection(void);
void ExitCriticalSection(void);
void ChangeClearRCnt(int, int);
void SysDeqIntRP(int, void *);
void SysEnqIntRP(int, void *);

void MemCard_StartCounterIrq(void) {
    register void *queue asm("$16");
    register int *regs asm("$3");
    register int *tail asm("$2");
    register int counter asm("$4");
    int value;

    D_8009B75C = 0;
    EnterCriticalSection();
    queue = &D_800A5AB0;
    SysDeqIntRP(2, queue);
    SysEnqIntRP(2, queue);

    counter = 3;
    regs = D_8009B784;
    asm volatile("" : : "r"(counter));
    value = -2;
    regs[0] = value;
    value = regs[1];
    value |= 1;
    regs[1] = value;
    ChangeClearRCnt(counter, 0);
    ExitCriticalSection();

    D_8009B728(D_8009B758);
    D_8009B728(D_8009B758 + 0xF0);
    tail = (int *)D_800A5AC0;
    asm volatile("" : "=r"(tail) : "0"(tail));
    tail[1] = 0;
    tail[0] = 0;
    D_8009B75C = 1;
}
