#ifndef PE1_MEMCARD_STATE_H
#define PE1_MEMCARD_STATE_H

#include "common.h"

/* Initialized to 0x1F801070: I_STAT followed by I_MASK.
 * The ordinary RAM counters at 0x800A5AC0/4 are separate objects. */
typedef struct MemCardInterruptRegisters {
    volatile unsigned int status;
    volatile unsigned int mask;
} MemCardInterruptRegisters;

extern MemCardInterruptRegisters * volatile g_MemCardState;

/* SIO0 at 0x1F801040. Status is accessed as its low halfword here. */
typedef struct MemCardSioRegisters {
    volatile unsigned char data;
    unsigned char reserved01[3];
    volatile unsigned short status;
    volatile unsigned short statusHigh;
    volatile unsigned short mode;
    volatile unsigned short control;
    unsigned short reserved0C;
    volatile unsigned short baud;
} MemCardSioRegisters;

PE1_STATIC_ASSERT(PE1_OFFSETOF(MemCardSioRegisters, status) == 4, card_sio_status);
PE1_STATIC_ASSERT(PE1_OFFSETOF(MemCardSioRegisters, control) == 10, card_sio_control);
PE1_STATIC_ASSERT(PE1_OFFSETOF(MemCardSioRegisters, baud) == 14, card_sio_baud);
PE1_STATIC_ASSERT(sizeof(MemCardSioRegisters) == 16, card_sio_size);
extern MemCardSioRegisters * volatile g_MemCardSioRegs;

#endif
