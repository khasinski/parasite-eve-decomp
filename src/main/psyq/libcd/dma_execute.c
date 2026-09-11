/* ASSEMBLER: GNU */
/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses -fno-schedule-insns */
#include "pe1/psyq_bios.h"
#include "pe1/psyq_cd.h"

void dma_execute(int channel, u32 address, int blockCount, int blockSize,
                 volatile u32 control, u8 interrupt) {
    int i = 0;
    register CdDmaInterruptRegister *interruptRegister asm("$3");
    register u32 bits asm("$2");
    int newBlockSize;
    u32 blockControl;
    register int shift asm("$6");
    volatile u32 readback;
    int priorityBit;
    volatile u32 *dma;

    while (*(volatile u32 *)(0x1F801088 + (channel << 4)) & 0x01000000) {
        if (i == 0x10000) {
            printf(D_80011C2C,
                   *(volatile u32 *)(0x1F801088 + (channel << 4)));
            break;
        }
        i++;
    }

    bits = 1;
    if (interrupt == bits) {
        interruptRegister = D_8009B348;
        bits = interruptRegister->bytes[2] | (bits << channel);
    } else {
        interruptRegister = D_8009B348;
        bits = interruptRegister->bytes[2] & ~(bits << channel);
    }
    interruptRegister->bytes[2] = bits;
    if (channel) {
        readback = D_8009B348->word;
    } else {
        readback = D_8009B348->word;
    }

    shift = channel << 2;
    asm volatile("" : "=r"(shift) : "0"(shift));
    shift += 3;
    newBlockSize = blockSize;
    priorityBit = 1 << shift;
    shift = *D_8009B344;
    dma = (volatile u32 *)0x1F801080;
    dma += channel << 2;
    blockControl = (blockCount << 16) | newBlockSize;
    *D_8009B344 = shift | priorityBit;
    *dma++ = address;
    *dma++ = blockControl;

    while (!(*D_8009B32C & 0x40)) {
    }

    if (address) {
        *dma = control;
        readback = *dma;
    } else {
        *dma = control;
        readback = *dma;
    }
}
