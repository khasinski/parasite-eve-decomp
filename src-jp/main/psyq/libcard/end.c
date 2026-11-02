/* ASSEMBLER: GNU */
/* PSY-Q LIBCARD END: _ExitCard. */
/*
 * PSY-Q LIBCARD END: _ExitCard. Assembler source with the same static ra
 * slot and inline C0 table call as LIBCARD PATCH. It copies the three-NOP
 * template g_CardExitHookTemplate..g_CardExitHookTemplateEnd over the card hook in the C0 table. That
 * template and END's final word are data at their link address, kept as the
 * segment psyq/libcard/END_templates.
 */

#include "pe1/psyq_asm.h"

PSYQ_ASM_OBJECT(LIBCARD, END)

PSYQ_ASM_FUNCTION(_ExitCard,
    "    lui     $at, %hi(g_ExitCardSavedRa)\n"
    "    sw      $ra, %lo(g_ExitCardSavedRa)($at)\n"
    "    jal     EnterCriticalSection\n"
    "    nop\n"
    "    addiu   $t1, $zero, 0x56\n"
    "    addiu   $t2, $zero, 0xB0\n"
    "    jalr    $t2\n"
    "    nop\n"
    "    lw      $v0, 0x18($v0)\n"
    "    lui     $t2, %hi(g_CardExitHookTemplate)\n"
    "    addiu   $t2, $t2, %lo(g_CardExitHookTemplate)\n"
    "    lui     $t1, %hi(g_CardExitHookTemplateEnd)\n"
    "    addiu   $t1, $t1, %lo(g_CardExitHookTemplateEnd)\n"
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
    "    lui     $ra, %hi(g_ExitCardSavedRa)\n"
    "    lw      $ra, %lo(g_ExitCardSavedRa)($ra)\n"
    "    nop\n"
    "    jr      $ra\n"
    "    nop\n");
