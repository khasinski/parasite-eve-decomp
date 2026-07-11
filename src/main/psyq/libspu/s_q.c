#include "pe1/psyq_spu_internal.h"

extern void EnterCriticalSection(void);
extern void ExitCriticalSection(void);
extern void CloseEvent(s32 event);
extern void DisableEvent(s32 event);
extern void _SpuDataCallback(SpuCallback callback);

extern s32 D_8009B384;
extern s32 D_8009B434;
extern s32 D_8009B438;

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
        D_8009B434 = 0;
        __asm__ volatile(
            "\t.set\tnoreorder\n"
            "\t.set\tnomacro\n"
            "lui $1, %hi(D_8009B438)\n"
            "jal _SpuDataCallback\n"
            "sw $zero, %lo(D_8009B438)($1)\n"
            "\t.set\tmacro\n"
            "\t.set\treorder\n");
        CloseEvent(D_8009B384);
        DisableEvent(D_8009B384);
        ExitCriticalSection();
    }
}
