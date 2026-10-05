#ifndef PE1_GTE_WINDOW_H
#define PE1_GTE_WINDOW_H

/*
 * Load the GTE rotation and translation registers from the view matrix.
 *
 * A few effect callbacks load RT0..RT4 and TRX..TRZ straight from the
 * matrix that the view slot D_800BCFA4 points to. Retail reads the slot
 * pointer once with `la v0,D_800BCFA4; lw base,0(v0)`, waits one
 * load-delay `nop`, then moves the eight matrix words through $t4..$t6 into
 * the GTE control registers:
 *
 *     lw t4,0(base); lw t5,4(base); ctc2 t4,$0; ctc2 t5,$1
 *     lw t4,8(base); lw t5,12(base); lw t6,16(base)
 *     ctc2 t4,$2; ctc2 t5,$3; ctc2 t6,$4              <- rotation window
 *     lw t4,20(base); lw t5,24(base); ctc2 t4,$5
 *     lw t6,28(base); ctc2 t5,$6; ctc2 t6,$7          <- translation window
 *
 * C reads of the eight words compile to different code: GCC folds the slot
 * address into an absolute `lui`/`lw`, keeps the pointer in $v0 and
 * schedules the word loads with the surrounding code, and the pins and
 * barriers needed to steer it back still miss retail's reload registers
 * elsewhere in the function. So these two windows are inline assembly.
 *
 * Each macro holds only the loads and `ctc2`s of its window. `matrix` is an
 * ordinary C operand, the pointer read from the slot, and the two macros are
 * used back to back with the same operand:
 *
 *     GTE_LOAD_ROTATION_WINDOW(D_800BCFA4.value);
 *     GTE_LOAD_TRANSLATION_WINDOW(D_800BCFA4.value);
 *
 * The compiler emits the pointer load: the two reads of the slot share its
 * address, which gives retail's `la v0`/`lw base,0(v0)`, and the second read
 * is a common subexpression, so both windows use the same base register
 * ($t0 or $t1, chosen by the allocator). The assembler adds the load-delay
 * `nop`. $t4..$t6 ($12..$14) are the transfer registers retail uses. Only
 * functions listed under "gte_matrix_windows" in
 * configs/USA/original_asm_evidence.json may use them, each the listed number
 * of times; see docs/ASM_AND_GTE_POLICY.md ("GTE matrix window").
 */
#define GTE_LOAD_ROTATION_WINDOW(matrix) \
    __asm__ volatile("lw $12,0(%0)\n" \
                     "lw $13,4(%0)\n" \
                     "ctc2 $12,$0\n" \
                     "ctc2 $13,$1\n" \
                     "lw $12,8(%0)\n" \
                     "lw $13,12(%0)\n" \
                     "lw $14,16(%0)\n" \
                     "ctc2 $12,$2\n" \
                     "ctc2 $13,$3\n" \
                     "ctc2 $14,$4" \
                     : : "r"(matrix) : "$12", "$13", "$14")

#define GTE_LOAD_TRANSLATION_WINDOW(matrix) \
    __asm__ volatile("lw $12,20(%0)\n" \
                     "lw $13,24(%0)\n" \
                     "ctc2 $12,$5\n" \
                     "lw $14,28(%0)\n" \
                     "ctc2 $13,$6\n" \
                     "ctc2 $14,$7" \
                     : : "r"(matrix) : "$12", "$13", "$14")

#endif
