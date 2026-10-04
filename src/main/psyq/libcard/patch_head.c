/* ASSEMBLER: GNU */
/*
 * PSY-Q LIBCARD PATCH, offset 0: the first routine of the patch image that
 * _copy_memcard_patch (psyq/libcard/CopyMemcardPatch.c) copies to 0xDF80,
 * followed there by CardPatchFunctions. It runs inside the BIOS with v0/v1
 * supplied by the patched kernel code, not under the C calling convention.
 */
#include "pe1/psyq_asm.h"

PSYQ_ASM_OBJECT(LIBCARD, PATCH)

PSYQ_ASM_FUNCTION(func_8007E344,
    "    lhu     $t7, 0xA($v1)\n"
    "    lui     $t0, 0x0\n"
    "    or      $t8, $t7, $v0\n"
    "    ori     $t9, $t8, 0x12\n"
    "    sh      $t9, 0xA($v1)\n"
    "    addiu   $t0, $zero, 0x28\n"
    ".Ldelay:\n"
    "    addiu   $t0, $t0, -0x1\n"
    "    bnez    $t0, .Ldelay\n"
    "    nop\n"
    "    jr      $ra\n"
    "    nop\n");
