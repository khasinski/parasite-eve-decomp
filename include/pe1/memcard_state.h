#ifndef PE1_MEMCARD_STATE_H
#define PE1_MEMCARD_STATE_H

/* Initialized to 0x1F801070: I_STAT followed by I_MASK.
 * The ordinary RAM counters at 0x800A5AC0/4 are separate objects. */
typedef struct MemCardInterruptRegisters {
    volatile unsigned int status;
    volatile unsigned int mask;
} MemCardInterruptRegisters;

extern MemCardInterruptRegisters * volatile g_MemCardState;

#endif
