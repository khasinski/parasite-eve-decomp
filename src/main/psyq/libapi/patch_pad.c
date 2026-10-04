/* ASSEMBLER: GNU */
/*
 * PSY-Q LIBAPI PATCH: _patch_pad (Pad_DequeueHandler). EnablePAD and
 * DisablePAD from the same object are reconstructed in pad/Pad_Toggles.c.
 * Assembler source: ra is parked in a static word, the B0 table is fetched
 * inline through the t2/t1 BIOS call protocol, and the kernel pad hooks are
 * addressed with trapping addi.
 */
#include "pe1/psyq_asm.h"

PSYQ_ASM_OBJECT(LIBAPI, PATCH)

PSYQ_ASM_FUNCTION(Pad_DequeueHandler,
    "    lui     $at, %hi(D_800A34C0)\n"
    "    sw      $ra, %lo(D_800A34C0)($at)\n"
    "    jal     EnterCriticalSection\n"
    "    nop\n"
    "    addiu   $t1, $zero, 0x57\n"
    "    addiu   $t2, $zero, 0xB0\n"
    "    jalr    $t2\n"
    "    nop\n"
    "    lw      $v0, 0x16C($v0)\n"
    "    addiu   $t1, $zero, 0xB\n"
    "    addi    $v1, $v0, 0x884\n"
    "    lui     $at, %hi(jtbl_800A34C8)\n"
    "    sw      $v1, %lo(jtbl_800A34C8)($at)\n"
    "    addi    $v1, $v0, 0x894\n"
    "    lui     $at, %hi(jtbl_800A34CC)\n"
    "    sw      $v1, %lo(jtbl_800A34CC)($at)\n"
    ".Lclear:\n"
    "    sw      $zero, 0x594($v0)\n"
    "    addiu   $v0, $v0, 0x4\n"
    "    addiu   $t1, $t1, -0x1\n"
    "    bnez    $t1, .Lclear\n"
    "    nop\n"
    "    jal     FlushCache\n"
    "    nop\n"
    "    lui     $ra, %hi(D_800A34C0)\n"
    "    lw      $ra, %lo(D_800A34C0)($ra)\n"
    "    nop\n"
    "    jr      $ra\n"
    "    nop\n");
