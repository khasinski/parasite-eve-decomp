#include "pe1/psyq_spu_internal.h"

extern int EnterCriticalSection(void);
extern void ExitCriticalSection(void);
extern s32 CloseEvent(s32 event);
extern s32 DisableEvent(s32 event);


void SpuQuit(void) {
    if (_spu_isCalled == 1) {
        __asm__ volatile(
            "\t.set\tnoreorder\n"
            "\t.set\tnomacro\n"
            "lui $1, %hi(D_8009B3EC)\n"
            "jal EnterCriticalSection\n"
            "sw $zero, %lo(D_8009B3EC)($1)\n"
            "addu $a0, $zero, $zero\n"
            "\t.set\tmacro\n"
            "\t.set\treorder\n");
        _spu_transferCallback = 0;
        __asm__ volatile(
            "\t.set\tnoreorder\n"
            "\t.set\tnomacro\n"
            "lui $1, %hi(D_8009B438)\n"
            "jal _SpuDataCallback\n"
            "sw $zero, %lo(D_8009B438)($1)\n"
            "\t.set\tmacro\n"
            "\t.set\treorder\n");
        CloseEvent(_spu_EVdma);
        DisableEvent(_spu_EVdma);
        ExitCriticalSection();
    }
}
