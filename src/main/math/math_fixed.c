/* ASSEMBLER: GNU */
#include "pe1/game_asm.h"

/*
 * Round a 16.16 (or 24.8) fixed-point value to its integer part:
 * (value + half) >> shift.
 *
 * Original assembler (configs/USA/original_asm_evidence.json): both are the
 * assembler's expansion of `add $v0, $a0, 0x8000`, which loads the constant
 * into $at and uses the trapping `add`. GCC never allocates $at and always
 * emits `addu` for C addition.
 */
GAME_ASM_FUNCTION(Math_FixedRoundToInt,
    "    ori     $at, $zero, 0x8000\n"
    "    add     $v0, $a0, $at\n"
    "    jr      $ra\n"
    "    sra     $v0, $v0, 16\n");

GAME_ASM_FUNCTION(Math_FixedRoundToByte,
    "    ori     $at, $zero, 0x8000\n"
    "    add     $v0, $a0, $at\n"
    "    jr      $ra\n"
    "    sra     $v0, $v0, 8\n");
