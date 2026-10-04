#ifndef PE1_PSYQ_ASM_H
#define PE1_PSYQ_ASM_H

/*
 * Original PSY-Q SDK assembler objects.
 *
 * Some PSY-Q library objects were assembled from handwritten assembler source,
 * not compiled from C: they save ra in a static data word instead of a stack
 * frame, pass values through t0..t2 to private entries, call the BIOS tables
 * inline through t1/t2, use the trapping add/addi/sub forms that GCC never
 * emits, or carry instruction templates that are copied into the kernel.
 * For those objects, and only those, a C translation unit may reproduce the
 * original assembler text with the macros below. The per-object evidence is
 * listed in docs/ASM_AND_GTE_POLICY.md ("PSY-Q assembler objects").
 *
 * This header is never for game code. A source using it must live under
 * src/main/psyq/, declare its SDK object with PSYQ_ASM_OBJECT, and contain
 * no C function definitions; tools/scripts/check_source_policy.py enforces
 * that and checks the object against configs/USA/psyq_provenance.json.
 *
 * Bodies are the original instruction text, one instruction per string, with
 * symbolic %hi/%lo relocations and labels rather than encoded words. They are
 * assembled with .set noreorder and .set noat, so every delay slot and load
 * delay is written out exactly as in the SDK object. GTE commands use the
 * mnemonics from include/gte_macros.inc (rtps, mvmva, sqr, op, gpf, ...).
 *
 * Instruction templates that an object only compares or copies are data at
 * their link address. They stay main.yaml rodata segments in the
 * .psyq_text_data linker section, not PSYQ_ASM_FUNCTION bodies.
 */

/* Names the SDK library and object this file reproduces. Emits nothing. */
#define PSYQ_ASM_OBJECT(library, object)

/* One global routine of an SDK assembler object. */
#define PSYQ_ASM_FUNCTION(name, body) \
    __asm__( \
        ".section .text, \"ax\"\n" \
        ".include \"gte_macros.inc\"\n" \
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
