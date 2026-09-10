/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses -fno-schedule-insns */
typedef unsigned int u32;
typedef unsigned char u8;
typedef union { volatile u32 word; volatile u8 bytes[4]; } DmaInterrupt;
extern DmaInterrupt *D_8009B348;
extern volatile u32 *D_8009B344;
extern volatile u8 *D_8009B32C;
extern char D_80011C2C[];
extern int printf(const char *, ...);
void dma_execute(int channel, void *address, int blockCount, int blockSize, u32 control, u8 interrupt) {
    int i = 0;
    DmaInterrupt *intr;
    u32 bits;
    volatile u32 readback;
    int priority_bit;
    volatile u32 *dma;
    while (*(volatile u32 *)(0x1f801088 + (channel << 4)) & 0x1000000) {
        if (i == 0x10000) {
            printf(D_80011C2C, *(volatile u32 *)(0x1f801088 + (channel << 4)));
            break;
        }
        i++;
    }
    if (interrupt == 1) {
        intr = D_8009B348;
        bits = intr->bytes[2] | (1 << channel);
    } else {
        intr = D_8009B348;
        bits = intr->bytes[2] & ~(1 << channel);
    }
    intr->bytes[2] = bits;
    readback = D_8009B348->word;
    priority_bit = 1 << ((channel << 2) + 3);
    dma = (volatile u32 *)(0x1f801080 + (channel << 4));
    *D_8009B344 |= priority_bit;
    *dma++ = (u32)address;
    *dma++ = (blockCount << 16) | blockSize;
    while (!(*D_8009B32C & 0x40)) {}
    *dma = control;
    readback = *dma;
}
