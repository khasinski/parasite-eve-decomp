/* CC1_FLAGS: -fno-schedule-insns2 */

extern int D_8009B75C;
extern void *D_800A5AB0;
extern int *D_8009B784;
extern int D_8009B758;
extern void (*D_8009B728)(int);
extern char D_800A5AC0[];

typedef struct MemCardIrqDataPage {
    int active;
    char pad_B760[0x48A0];
} MemCardIrqDataPage;

register MemCardIrqDataPage *g_MemCardIrqDataPage asm("$1");

void EnterCriticalSection(void);
void ExitCriticalSection(void);
void ChangeClearRCnt(int, int);
void SysDeqIntRP(int, void *);
void SysEnqIntRP(int, void *);

void MemCard_StartCounterIrq(void) {
    void *queue;
    register int *regs asm("$3");
    int *tail;
    register int counter asm("$4");
    int value;

    g_MemCardIrqDataPage = (MemCardIrqDataPage *)0x800A0000;
    g_MemCardIrqDataPage[-1].active = 0;
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
