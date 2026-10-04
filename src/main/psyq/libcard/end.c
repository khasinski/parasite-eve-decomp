/* ASSEMBLER: GNU */
/*
 * PSY-Q LIBCARD END: _ExitCard. Assembler source with the same static ra
 * slot and inline C0 table call as LIBCARD PATCH. It copies the three-NOP
 * template D_8007E584..D_8007E590 over the card hook in the C0 table. That
 * template and END's final word are data at their link address, kept as the
 * segment psyq/libcard/END_templates.
 */
#include "pe1/psyq_asm.h"

PSYQ_ASM_OBJECT(LIBCARD, END)

PSYQ_ASM_FUNCTION(_ExitCard,
    "    lui     $at, %hi(D_800A34F0)\n"
    "    sw      $ra, %lo(D_800A34F0)($at)\n"
    "    jal     EnterCriticalSection\n"
    "    nop\n"
    "    addiu   $t1, $zero, 0x56\n"
    "    addiu   $t2, $zero, 0xB0\n"
    "    jalr    $t2\n"
    "    nop\n"
    "    lw      $v0, 0x18($v0)\n"
    "    lui     $t2, %hi(D_8007E584)\n"
    "    addiu   $t2, $t2, %lo(D_8007E584)\n"
    "    lui     $t1, %hi(D_8007E590)\n"
    "    addiu   $t1, $t1, %lo(D_8007E590)\n"
    ".Lcopy:\n"
    "    lw      $v1, 0x0($t2)\n"
    "    nop\n"
    "    sw      $v1, 0x70($v0)\n"
    "    addiu   $t2, $t2, 0x4\n"
    "    bne     $t2, $t1, .Lcopy\n"
    "    addiu   $v0, $v0, 0x4\n"
    "    jal     FlushCache\n"
    "    nop\n"
    "    jal     ExitCriticalSection\n"
    "    nop\n"
    "    lui     $ra, %hi(D_800A34F0)\n"
    "    lw      $ra, %lo(D_800A34F0)($ra)\n"
    "    nop\n"
    "    jr      $ra\n"
    "    nop\n");
