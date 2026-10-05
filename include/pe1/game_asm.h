#ifndef PE1_GAME_ASM_H
#define PE1_GAME_ASM_H

/*
 * Original game-side assembler routines.
 *
 * A few small game helpers were written in assembler, not compiled from C:
 * they use the trapping add/addi/sub forms that GCC never emits, load
 * constants into $at, keep ra in a scratch register instead of a stack
 * frame, or read HI/LO directly in an order and register assignment that the
 * compiler's long-long patterns cannot produce. For those routines, and only
 * those, a C translation unit may reproduce the original instruction text
 * with GAME_ASM_FUNCTION.
 *
 * This is not a fallback for functions that are hard to match. Every routine
 * must be listed with its address, size and evidence in
 * configs/USA/original_asm_evidence.json, and the unit's yaml range must be
 * exactly the listed routines. tools/scripts/check_source_policy.py enforces
 * that, plus the GNU assembler marker and the absence of C functions in the
 * same file. Policy and per-function evidence: docs/ASM_AND_GTE_POLICY.md
 * ("Game-side assembler").
 *
 * Bodies are the original instruction text, one instruction per string, with
 * symbolic call targets, %hi/%lo relocations and local labels rather than
 * encoded words. They are assembled with .set noreorder and .set noat, so
 * every delay slot, hazard NOP and $at use is written out as it appears in
 * the retail binary. The macro works in the main executable and in overlays.
 */

/* One global routine of original game assembler. */
#define GAME_ASM_FUNCTION(name, body) \
    __asm__( \
        ".section .text, \"ax\"\n" \
        ".set push\n" \
        ".set noreorder\n" \
        ".set noat\n" \
        ".globl " #name "\n" \
        ".type " #name ", @function\n" \
        ".ent " #name "\n" \
        #name ":\n" \
        body \
        ".end " #name "\n" \
        ".size " #name ", . - " #name "\n" \
        ".set pop\n")

#endif
