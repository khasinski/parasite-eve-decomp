/* MASPSX_FLAGS: --store-return-delay */
#include "include_asm.h"

#include "include_asm.h"

#include "pe1/memcard_state.h"

extern int (*D_8009B74C)(void);
typedef int (*MemCardStepFn)(void);
typedef void (*MemCardErrorFn)(int);

extern int g_MemCardCallbackPending;
extern int D_8009B75C;
extern int D_8009B758;
extern int D_8009B764;
extern int D_8009B768;
extern int D_8009B76C;
extern int D_8009B774;
extern int D_8009B778;
extern int D_8009B78C;
extern void *D_8009B788;
extern int D_800A5AC4;
extern MemCardStepFn D_8009B7A8[];
extern MemCardErrorFn D_8009B724;

extern void EnterCriticalSection(void);
extern void ExitCriticalSection(void);
extern void ChangeClearRCnt(int, int);
extern void SysDeqIntRP(int, void *);
extern int g_MemCardCounterIrqQueueNode;

typedef unsigned short u16;

typedef struct {
    unsigned char field0;
    unsigned char pad1[3];
    u16 field4;
    u16 pad6;
    u16 pad8;
    u16 fieldA;
} TimerRegs;

extern TimerRegs * volatile g_MemCardSioRegs;

int Spu_CheckTimerElapsed(void);
int MemCard_DmaProcess();
void MemCard_RunCommandStep();

int MemCard_TimerReadyCallback(void) {
    MemCardState *state = g_MemCardState;

    if ((state->field4 & 1) == 0) {
        return 0;
    }
    if ((state->field0 & 1) == 0) {
        return 0;
    }
    if (D_8009B74C != 0) {
        D_8009B74C();
    }
    return 1;
}

int MemCard_TimerCallback(void) {
    int index;
    int limit;
    register int *timer asm("$4");
    void *obj;
    register int active asm("$3");
    register int one asm("$2");

    active = D_8009B774;
    one = 1;
    asm volatile(
        ".set noat\n"
        "lui $1,%%hi(D_8009B78C)\n"
        "beqz %0,1f\n"
        "sw %1,%%lo(D_8009B78C)($1)\n"
        "lui $4,%%hi(D_800A5AC0)\n"
        "addiu $4,$4,%%lo(D_800A5AC0)\n"
        "lw $3,0($4)\n"
        "nop\n"
        "slti $2,$3,0x96\n"
        "beqz $2,1f\n"
        "addiu $2,$3,1\n"
        "sw $2,0($4)\n"
        "1:\n"
        ".set at"
        :
        : "r"(active), "r"(one)
        : "$1", "$2", "$3", "$4", "memory");

    if (D_8009B778 == 0) {
        timer = &D_800A5AC4;
        if (*timer < 0x96) {
            *timer += 1;
        }
    }

    if (D_8009B75C != 0) {
        index = D_8009B774;
        limit = D_8009B778;
        if (limit >= index) {
            obj = (void *)(D_8009B758 + (((index << 4) - index) << 4));
            D_8009B768 = 0;
            D_8009B764 = index;
            if (MemCard_DmaProcess(obj) == 0) {
                D_8009B724(0xFFFF);
            }

            D_8009B76C = 0;
            while (D_8009B778 >= D_8009B764) {
                index = D_8009B764;
                obj = (void *)(D_8009B758 + (((index << 4) - index) << 4));
                MemCard_RunCommandStep(obj);
            }
            *(short *)((char *)D_8009B788 + 0xE) = 0x88;
        }
    }

    return 0;
}

int MemCard_TakeCallback(void) {
    int old = g_MemCardCallbackPending;
    g_MemCardCallbackPending = 0;
    return old;
}
