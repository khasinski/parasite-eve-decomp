/* ASSEMBLER: GNU */
/*
 * PSY-Q LIBAPI CHCLRPAD: _remove_ChgclrPAD (Pad_StopHandler). Assembler
 * source with the same static ra slot, inline B0 table call and trapping addi
 * as LIBAPI PATCH.
 */
#include "pe1/psyq_asm.h"

PSYQ_ASM_OBJECT(LIBAPI, CHCLRPAD)

PSYQ_ASM_FUNCTION(Pad_StopHandler,
    "    lui     $at, %hi(D_800A34D0)\n"
    "    sw      $ra, %lo(D_800A34D0)($at)\n"
    "    jal     EnterCriticalSection\n"
    "    nop\n"
    "    addiu   $t1, $zero, 0x57\n"
    "    addiu   $t2, $zero, 0xB0\n"
    "    jalr    $t2\n"
    "    nop\n"
    "    addiu   $t2, $zero, 0x9\n"
    "    lw      $v0, 0x16C($v0)\n"
    "    nop\n"
    "    addi    $v1, $v0, 0x62C\n"
    ".Lclear:\n"
    "    sw      $zero, 0x0($v1)\n"
    "    addiu   $v1, $v1, 0x4\n"
    "    addiu   $t2, $t2, -0x1\n"
    "    bnez    $t2, .Lclear\n"
    "    nop\n"
    "    jal     FlushCache\n"
    "    nop\n"
    "    jal     ExitCriticalSection\n"
    "    nop\n"
    "    lui     $ra, %hi(D_800A34D0)\n"
    "    lw      $ra, %lo(D_800A34D0)($ra)\n"
    "    nop\n"
    "    jr      $ra\n"
    "    nop\n");
