#ifndef PE1_GTE_H
#define PE1_GTE_H

/*
 * PSY-Q-style GTE operations.  Keep these macros limited to COP2 state and
 * commands; the calling function owns all normal CPU-side algorithmic work.
 */

/* Load the five packed words which hold R11..R33. */
#define gte_ldrotmatrix(matrix) \
    asm volatile("lw $12,0(%0)\n\t" \
                 "lw $13,4(%0)\n\t" \
                 "ctc2 $12,$0\n\t" \
                 "ctc2 $13,$1\n\t" \
                 "lw $12,8(%0)\n\t" \
                 "lw $13,12(%0)\n\t" \
                 "lw $14,16(%0)\n\t" \
                 "ctc2 $12,$2\n\t" \
                 "ctc2 $13,$3\n\t" \
                 "ctc2 $14,$4" \
                 : : "r"(matrix) : "$12", "$13", "$14")

#define gte_ldv0(vec) \
    asm volatile("lwc2 $0,0(%0)\n\t" \
                 "lwc2 $1,4(%0)" \
                 : : "r"(vec) : "memory")

/* MVMVA: rotation matrix, V0, no translation, SF=0, LM=0. */
#define gte_mvmva() \
    asm volatile("nop\n\t" \
                 "nop\n\t" \
                 ".word 0x4A486012")

#define gte_stmac(out) \
    asm volatile("swc2 $25,0(%0)\n\t" \
                 "swc2 $26,4(%0)\n\t" \
                 "swc2 $27,8(%0)" \
                 : : "r"(out) : "memory")

#endif
