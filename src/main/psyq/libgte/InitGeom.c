/* ASSEMBLER: GNU */
/*
 * PSY-Q LIBGTE MSC00: InitGeom. Assembler source: ra is parked in the
 * object's static SAVERA word around the PATCHGTE installer call, COP2 is
 * enabled through the status register, and the GTE control registers are
 * written directly with hand-placed hazard NOPs. PSY-Q 3.5 msc00.o has the
 * same 32 instruction words. The 8-byte object signature before InitGeom is
 * the separate psyq/libgte/MSC00_signature segment.
 */
#include "pe1/psyq_asm.h"

PSYQ_ASM_OBJECT(LIBGTE, MSC00)

PSYQ_ASM_FUNCTION(InitGeom,
    "    lui     $at, %hi(D_800960AC)\n"
    "    sw      $ra, %lo(D_800960AC)($at)\n"
    "    jal     St_InstallDmaHandler\n"
    "    nop\n"
    "    lui     $ra, %hi(D_800960AC)\n"
    "    lw      $ra, %lo(D_800960AC)($ra)\n"
    "    nop\n"
    "    mfc0    $v0, $12\n"
    "    lui     $v1, 0x4000\n"
    "    or      $v0, $v0, $v1\n"
    "    mtc0    $v0, $12\n"
    "    nop\n"
    "    addiu   $t0, $zero, 0x155\n"
    "    ctc2    $t0, $29\n"
    "    nop\n"
    "    addiu   $t0, $zero, 0x100\n"
    "    ctc2    $t0, $30\n"
    "    nop\n"
    "    addiu   $t0, $zero, 0x3E8\n"
    "    ctc2    $t0, $26\n"
    "    nop\n"
    "    addiu   $t0, $zero, -0x1062\n"
    "    ctc2    $t0, $27\n"
    "    nop\n"
    "    lui     $t0, 0x140\n"
    "    ctc2    $t0, $28\n"
    "    nop\n"
    "    ctc2    $zero, $24\n"
    "    ctc2    $zero, $25\n"
    "    nop\n"
    "    jr      $ra\n"
    "    nop\n");
