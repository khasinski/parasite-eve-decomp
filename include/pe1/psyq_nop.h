#ifndef PE1_PSYQ_NOP_H
#define PE1_PSYQ_NOP_H

/* Explicit retail scheduling slots. Each macro emits exactly one NOP.
 * Authorized exception to CPU instruction asm; counted as nop_barriers debt.
 * Dependencies keep the slot adjacent to the relevant C calculation/load.
 */
#define PE1_NOP() asm volatile("nop")
#define PE1_NOP_DEP(constraint, value) asm volatile("nop" : : constraint(value))
#define PE1_NOP_MEMORY_DEP(constraint, value) \
    asm volatile("nop" : : constraint(value) : "memory")
/* Keep two live arguments and a preceding result across a scheduling slot. */
#define PE1_NOP_IO2_DEP(first, second, value) \
    asm volatile("nop" : "=r"(first), "=r"(second) \
                 : "r"(value), "0"(first), "1"(second))

#endif
