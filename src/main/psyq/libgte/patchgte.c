/* ASSEMBLER: GNU */
/*
 * PSY-Q LIBGTE PATCHGTE: _patch_gte. Assembler
 * source: ra is parked in a static word, the B0 table is fetched inline
 * through the t2/t1 BIOS call protocol, and the copy loops run over
 * instruction templates kept in the object's text section. The installer
 * compares the kernel exception prologue with LIBGTE_PATCHGTE_text_AC and, on
 * a match, overwrites it with LIBGTE_PATCHGTE_text_C4; LIBGTE_PATCHGTE_text_DC ends the second
 * template. The templates are never executed at their link address, so they
 * are the data segment psyq/libgte/PATCHGTE_templates. PSY-Q 3.5 patchgte.o
 * has the same structure (_patch_GTE.._patch_GTE_end in .text, copy loop at
 * assembler label 1$) without the comparison pass.
 */
#include "pe1/psyq_asm.h"

PSYQ_ASM_OBJECT(LIBGTE, PATCHGTE)

PSYQ_ASM_FUNCTION(_patch_gte,
    "    lui     $at, %hi(D_800A3450)\n"
    "    sw      $ra, %lo(D_800A3450)($at)\n"
    "    jal     EnterCriticalSection\n"
    "    nop\n"
    "    addiu   $t1, $zero, 0x56\n"
    "    addiu   $t2, $zero, 0xB0\n"
    "    jalr    $t2\n"
    "    nop\n"
    "    lw      $v0, 0x18($v0)\n"
    "    nop\n"
    "    addiu   $v0, $v0, 0x28\n"
    "    addu    $t7, $v0, $zero\n"
    "    lui     $t2, %hi(LIBGTE_PATCHGTE_text_AC)\n"
    "    addiu   $t2, $t2, %lo(LIBGTE_PATCHGTE_text_AC)\n"
    "    lui     $t1, %hi(LIBGTE_PATCHGTE_text_C4)\n"
    "    addiu   $t1, $t1, %lo(LIBGTE_PATCHGTE_text_C4)\n"
    ".Lcompare:\n"
    "    lw      $v1, 0x0($t2)\n"
    "    lw      $t3, 0x0($v0)\n"
    "    addiu   $t2, $t2, 0x4\n"
    "    bne     $v1, $t3, .Ldone\n"
    "    addiu   $v0, $v0, 0x4\n"
    "    bne     $t2, $t1, .Lcompare\n"
    "    nop\n"
    "    addu    $v0, $t7, $zero\n"
    "    lui     $t2, %hi(LIBGTE_PATCHGTE_text_C4)\n"
    "    addiu   $t2, $t2, %lo(LIBGTE_PATCHGTE_text_C4)\n"
    "    lui     $t1, %hi(LIBGTE_PATCHGTE_text_DC)\n"
    "    addiu   $t1, $t1, %lo(LIBGTE_PATCHGTE_text_DC)\n"
    ".Lcopy:\n"
    "    lw      $v1, 0x0($t2)\n"
    "    nop\n"
    "    sw      $v1, 0x0($v0)\n"
    "    addiu   $t2, $t2, 0x4\n"
    "    bne     $t2, $t1, .Lcopy\n"
    "    addiu   $v0, $v0, 0x4\n"
    ".Ldone:\n"
    "    jal     FlushCache\n"
    "    nop\n"
    "    jal     ExitCriticalSection\n"
    "    nop\n"
    "    lui     $ra, %hi(D_800A3450)\n"
    "    lw      $ra, %lo(D_800A3450)($ra)\n"
    "    nop\n"
    "    jr      $ra\n"
    "    nop\n");
