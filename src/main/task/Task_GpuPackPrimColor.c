/* ASSEMBLER: GNU */
#include "pe1/game_asm.h"

/*
 * int Task_GpuPackPrimColor(int low, int high): low plus the 16-bit
 * Task_GpuFlushPrimQueue result scaled to (high - low), i.e.
 * low + ((u16)r * (high - low) >> 16).
 *
 * Original assembler (configs/USA/original_asm_evidence.json): ra is kept in
 * $v1 with `or` instead of a stack frame across the call, and the range and
 * result use the trapping `sub` and `add`, which GCC never emits.
 */
GAME_ASM_FUNCTION(Task_GpuPackPrimColor,
    "    or      $v1, $zero, $ra\n"
    "    jal     Task_GpuFlushPrimQueue\n"
    "    sub     $a1, $a1, $a0\n"
    "    andi    $v0, $v0, 0xFFFF\n"
    "    mult    $v0, $a1\n"
    "    or      $ra, $zero, $v1\n"
    "    mflo    $v0\n"
    "    mfhi    $t3\n"
    "    srl     $v0, $v0, 16\n"
    "    sll     $t3, $t3, 16\n"
    "    or      $v0, $v0, $t3\n"
    "    jr      $ra\n"
    "    add     $v0, $v0, $a0\n");
