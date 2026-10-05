/* ASSEMBLER: GNU */
#include "pe1/game_asm.h"

/*
 * unsigned int Math_SqrtApprox3(int x, int y, int z): bits 20..51 of the
 * 64-bit sum x*x + y*y + z*z, computed on the absolute values.
 *
 * Original assembler (configs/USA/original_asm_evidence.json): no stack
 * frame, LO read before HI, two hazard NOPs before each following `mult`,
 * unfilled `bgez` delay slots, and a carry chain that adds the carry before
 * the high words. GCC's long-long code for the same sum spills to a stack
 * frame, reads HI first, fills the delay slots and keeps the dead high-word
 * shift.
 */
GAME_ASM_FUNCTION(Math_SqrtApprox3,
    "    bgez    $a0, .LMath_SqrtApprox3_x_positive\n"
    "    nop\n"
    "    negu    $a0, $a0\n"
    ".LMath_SqrtApprox3_x_positive:\n"
    "    bgez    $a1, .LMath_SqrtApprox3_y_positive\n"
    "    nop\n"
    "    negu    $a1, $a1\n"
    ".LMath_SqrtApprox3_y_positive:\n"
    "    bgez    $a2, .LMath_SqrtApprox3_z_positive\n"
    "    nop\n"
    "    negu    $a2, $a2\n"
    ".LMath_SqrtApprox3_z_positive:\n"
    "    mult    $a0, $a0\n"
    "    mflo    $a3\n"
    "    mfhi    $t0\n"
    "    nop\n"
    "    nop\n"
    "    mult    $a1, $a1\n"
    "    mflo    $t1\n"
    "    mfhi    $t2\n"
    "    nop\n"
    "    nop\n"
    "    mult    $a2, $a2\n"
    "    mflo    $a0\n"
    "    mfhi    $a1\n"
    "    addu    $v0, $a3, $t1\n"
    "    sltu    $v1, $v0, $a3\n"
    "    addu    $v1, $v1, $t0\n"
    "    addu    $v1, $v1, $t2\n"
    "    addu    $a2, $v0, $a0\n"
    "    sltu    $a3, $a2, $v0\n"
    "    addu    $a3, $a3, $v1\n"
    "    addu    $a3, $a3, $a1\n"
    "    srl     $a2, $a2, 20\n"
    "    sll     $a3, $a3, 12\n"
    "    jr      $ra\n"
    "    or      $v0, $a3, $a2\n");
