/* MASPSX_FLAGS: --la-call-delay */
#include "pe1/psyq_spu_internal.h"

extern void EnterCriticalSection(void);
extern void ExitCriticalSection(void);
extern s32 OpenEvent(s32, s32, s32, void *);
extern void EnableEvent(s32);
extern void _SpuDataCallback(SpuCallback callback);
extern void _spu_FiDMA(void);

extern s32 D_8009B384;

void SpuStart(void) {
    register s32 event asm("$4");

    if (_spu_isCalled == 0) {
        __asm__ volatile(
            "\t.set\tnoreorder\n"
            "\t.set\tnomacro\n"
            "addiu $v0, $zero, 1\n"
            "lui $1, %hi(D_8009B3EC)\n"
            "jal EnterCriticalSection\n"
            "sw $v0, %lo(D_8009B3EC)($1)\n"
            "\t.set\tmacro\n"
            "\t.set\treorder\n");
        _SpuDataCallback(_spu_FiDMA);
        event = OpenEvent(0xF0000009, 0x20, 0x2000, 0);
        asm volatile("" : : "r"(event));
        __asm__ volatile(
            "\t.set\tnoreorder\n"
            "\t.set\tnomacro\n"
            "lui $1, %hi(D_8009B384)\n"
            "jal EnableEvent\n"
            "sw $a0, %lo(D_8009B384)($1)\n"
            "\t.set\tmacro\n"
            "\t.set\treorder\n");
        ExitCriticalSection();
    }
}
