#ifndef PE1_PSYQ_BIOS_H
#define PE1_PSYQ_BIOS_H

#define PE1_STRINGIFY_INNER(value) #value
#define PE1_STRINGIFY(value) PE1_STRINGIFY_INNER(value)

/* PSY-Q's assembled BIOS veneers preserve all arguments and tail-call the
 * selected A0/B0/C0 service through the documented t2/t1 register ABI. */
#define PSYQ_BIOS_TRAMPOLINE(name, table, selector) \
    __asm__( \
        ".section .text, \"ax\"\n" \
        ".set noreorder\n" \
        ".globl " #name "\n" \
        ".type " #name ", @function\n" \
        #name ":\n" \
        ".ent " #name "\n" \
        "addiu $10, $0, " PE1_STRINGIFY(table) "\n" \
        "jr $10\n" \
        "addiu $9, $0, " PE1_STRINGIFY(selector) "\n" \
        ".size " #name ", . - " #name "\n" \
        ".end " #name "\n" \
        ".set reorder\n")

#endif
