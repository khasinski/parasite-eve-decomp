/* ASSEMBLER: GNU */
#include "pe1/game_asm.h"

/*
 * int Math_FixedMul(int a, int b): the middle 32 bits of the signed 64-bit
 * product, i.e. a 16.16 fixed-point multiply.
 *
 * Original assembler (configs/USA/original_asm_evidence.json): it reads LO
 * before HI and shifts both in place. GCC's mulsidi3/ashrdi3 patterns read HI
 * first and keep the dead high-word shift for `(long long)a * b >> 16`.
 */
GAME_ASM_FUNCTION(Math_FixedMul,
    "    mult    $a0, $a1\n"
    "    mflo    $v0\n"
    "    mfhi    $v1\n"
    "    srl     $v0, $v0, 16\n"
    "    sll     $v1, $v1, 16\n"
    "    jr      $ra\n"
    "    or      $v0, $v1, $v0\n");
