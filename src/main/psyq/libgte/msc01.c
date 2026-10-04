/* ASSEMBLER: GNU */
/*
 * PSY-Q LIBGTE MSC01: SquareRoot0 (project name Gte_ISqrt). Assembler source:
 * leading-zero count through the GTE LZCS/LZCR registers, trapping sub/addi,
 * and a branch whose delay slot is the first instruction of the next label.
 * PSY-Q 3.5 msc01.o carries the local labels Rshift, CNTSQ and RTNSQRT at the
 * same offsets; 31 of its 33 words are identical and the other two differ
 * only in the temporary register its assembler chose to expand `and $t2,
 * $v0, -2`.
 */
#include "pe1/psyq_asm.h"

PSYQ_ASM_OBJECT(LIBGTE, MSC01)

PSYQ_ASM_FUNCTION(Gte_ISqrt,
    "    mtc2    $a0, $30\n"
    "    nop\n"
    "    nop\n"
    "    mfc2    $v0, $31\n"
    "    addiu   $at, $zero, 0x20\n"
    "    beq     $v0, $at, .LRTNSQRT\n"
    "    nop\n"
    "    andi    $t0, $v0, 0x1\n"
    "    addiu   $at, $zero, -0x2\n"
    "    and     $t2, $v0, $at\n"
    "    addiu   $t1, $zero, 0x1F\n"
    "    sub     $t1, $t1, $t2\n"
    "    sra     $t1, $t1, 1\n"
    "    addi    $t3, $t2, -0x18\n"
    "    bltz    $t3, .LRshift\n"
    "    nop\n"
    "    sllv    $t4, $a0, $t3\n"
    "    b       .LCNTSQ\n"
    ".LRshift:\n"
    "    addiu   $t3, $zero, 0x18\n"
    "    sub     $t3, $t3, $t2\n"
    "    srav    $t4, $a0, $t3\n"
    ".LCNTSQ:\n"
    "    addi    $t4, $t4, -0x40\n"
    "    sll     $t4, $t4, 1\n"
    "    lui     $t5, %hi(D_800960BC)\n"
    "    addu    $t5, $t5, $t4\n"
    "    lh      $t5, %lo(D_800960BC)($t5)\n"
    "    nop\n"
    "    sllv    $t5, $t5, $t1\n"
    "    srl     $v0, $t5, 12\n"
    "    jr      $ra\n"
    "    nop\n"
    ".LRTNSQRT:\n"
    "    jr      $ra\n"
    "    addiu   $v0, $zero, 0x0\n");
