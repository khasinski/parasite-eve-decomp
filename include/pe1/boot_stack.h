#ifndef PE1_BOOT_STACK_H
#define PE1_BOOT_STACK_H

/*
 * Run one call on a stack in the 1 KiB scratchpad.
 *
 * The boot loop runs func_8019234C with $sp moved to the top word of the
 * scratchpad and puts the caller's $sp back afterwards. C has no way to
 * name or change the stack pointer, so this one window is inline assembly.
 *
 * The macro owns only the stack switch: it parks the old $sp in the top word,
 * points $sp just below it, and after the call steps back up and reloads the
 * old $sp. The top address is an ordinary C operand, and the call itself is
 * plain C between the two asm statements. $t0 is the transfer register the
 * retail code uses. Only functions listed under "stack_switch_macros" in
 * configs/USA/original_asm_evidence.json may use it; see
 * docs/ASM_AND_GTE_POLICY.md ("Scratchpad stack switch").
 */

/* Top word of the scratchpad (0x1F800000..0x1F8003FF). */
#define BOOT_SCRATCHPAD_STACK_TOP ((u32 *)0x1F8003FC)

#define BOOT_CALL_ON_SCRATCHPAD_STACK(top, call) \
    { \
        __asm__ volatile("move $8,%0\n" \
                         "sw $29,0($8)\n" \
                         "addiu $8,$8,-4\n" \
                         "move $29,$8" \
                         : : "r"(top) : "$8", "memory"); \
        call; \
        __asm__ volatile("addiu $29,$29,4\n" \
                         "lw $29,0($29)" \
                         : : : "memory"); \
    }

#endif
