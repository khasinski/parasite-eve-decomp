/* ASSEMBLER: GNU */
/*
 * PSY-Q LIBCARD PATCH, offsets 0x84..0x19C: the second redirect template
 * (text_84, func_8007E3C8) and the installers _patch_card (func_8007E3DC) and
 * _patch_card2 (func_8007E470). Assembler source: ra is parked in a static
 * word, the C0/B0 tables are fetched inline through the t2/t1 BIOS call
 * protocol, and the templates are copied into kernel memory word by word.
 * The template's fifth word is part of the 20 bytes _patch_card2 copies.
 */
#include "pe1/psyq_asm.h"

PSYQ_ASM_OBJECT(LIBCARD, PATCH)

PSYQ_ASM_FUNCTION(func_8007E3C8,
    "    lui     $t0, %hi(D_A000DF80)\n"
    "    addiu   $t0, $t0, %lo(D_A000DF80)\n"
    "    jalr    $t0\n"
    "    nop\n"
    "    nop\n");

PSYQ_ASM_FUNCTION(func_8007E3DC,
    "    lui     $at, %hi(D_800A34E0)\n"
    "    sw      $ra, %lo(D_800A34E0)($at)\n"
    "    jal     EnterCriticalSection\n"
    "    nop\n"
    "    addiu   $t1, $zero, 0x56\n"
    "    addiu   $t2, $zero, 0xB0\n"
    "    jalr    $t2\n"
    "    nop\n"
    "    lw      $v0, 0x18($v0)\n"
    "    nop\n"
    "    lw      $v1, 0x70($v0)\n"
    "    nop\n"
    "    andi    $t1, $v1, 0xFFFF\n"
    "    sll     $t1, $t1, 16\n"
    "    lw      $v1, 0x74($v0)\n"
    "    nop\n"
    "    andi    $t2, $v1, 0xFFFF\n"
    "    addu    $v1, $t1, $t2\n"
    "    addiu   $v0, $v1, 0x28\n"
    "    lui     $t2, %hi(func_8007E3B4)\n"
    "    addiu   $t2, $t2, %lo(func_8007E3B4)\n"
    "    lui     $t1, %hi(func_8007E3C8)\n"
    "    addiu   $t1, $t1, %lo(func_8007E3C8)\n"
    ".Lcopy_card:\n"
    "    lw      $v1, 0x0($t2)\n"
    "    nop\n"
    "    sw      $v1, 0x0($v0)\n"
    "    addiu   $t2, $t2, 0x4\n"
    "    bne     $t2, $t1, .Lcopy_card\n"
    "    addiu   $v0, $v0, 0x4\n"
    "    lui     $at, %hi(g_CardPatchContinuation)\n"
    "    jal     FlushCache\n"
    "    sw      $v0, %lo(g_CardPatchContinuation)($at)\n"
    "    lui     $ra, %hi(D_800A34E0)\n"
    "    lw      $ra, %lo(D_800A34E0)($ra)\n"
    "    nop\n"
    "    jr      $ra\n"
    "    nop\n");

PSYQ_ASM_FUNCTION(func_8007E470,
    "    lui     $at, %hi(D_800A34E0)\n"
    "    sw      $ra, %lo(D_800A34E0)($at)\n"
    "    jal     EnterCriticalSection\n"
    "    nop\n"
    "    addiu   $t1, $zero, 0x57\n"
    "    addiu   $t2, $zero, 0xB0\n"
    "    jalr    $t2\n"
    "    nop\n"
    "    lw      $v0, 0x16C($v0)\n"
    "    nop\n"
    "    lw      $v1, 0x9C8($v0)\n"
    "    lui     $t2, %hi(func_8007E3C8)\n"
    "    addiu   $t2, $t2, %lo(func_8007E3C8)\n"
    "    lui     $t1, %hi(func_8007E3DC)\n"
    "    addiu   $t1, $t1, %lo(func_8007E3DC)\n"
    ".Lcopy_card2:\n"
    "    lw      $t0, 0x0($t2)\n"
    "    nop\n"
    "    sw      $t0, 0x9C8($v0)\n"
    "    addiu   $t2, $t2, 0x4\n"
    "    bne     $t2, $t1, .Lcopy_card2\n"
    "    addiu   $v0, $v0, 0x4\n"
    "    jal     FlushCache\n"
    "    nop\n"
    "    lui     $ra, %hi(D_800A34E0)\n"
    "    lw      $ra, %lo(D_800A34E0)($ra)\n"
    "    nop\n"
    "    jr      $ra\n"
    "    nop\n");
