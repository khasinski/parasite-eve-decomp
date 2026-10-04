#ifndef PE1_DEAD_CODE_H
#define PE1_DEAD_CODE_H

/* Marks retail dead code that is kept only because its empty branch still
 * shapes instruction scheduling (the body is deleted before final output, but
 * the branch splits a scheduling block until after both scheduling passes).
 * Expands to the statements unchanged; counted as dead_code crutch debt. */
#define PE1_DEAD_CODE(statements) statements

#endif
