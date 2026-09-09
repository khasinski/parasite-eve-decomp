/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */
typedef unsigned int u32;
typedef unsigned char u8;
typedef struct { volatile u32 address, blocks, control, reserved; } DmaChannel;
typedef union { volatile u32 word; volatile u8 bytes[4]; } DmaInterrupt;
extern DmaInterrupt *D_8009B348;
extern volatile u32 *D_8009B344;
extern volatile u8 *D_8009B32C;
extern char D_80011C2C[];
extern int printf(const char *, ...);
void dma_execute(int channel, void *address, int blockCount, int blockSize, u32 control, u8 interrupt) {
    int i = 0;
    volatile u32 readback;
    volatile u32 *dma;
    while (((DmaChannel *)0x1f801080)[channel].control & 0x1000000) {
        if (i == 0x10000) {
            printf(D_80011C2C, ((DmaChannel *)0x1f801080)[channel].control);
            break;
        }
        i++;
    }
    if (interrupt == 1) D_8009B348->bytes[2] |= 1 << channel;
    else D_8009B348->bytes[2] &= ~(1 << channel);
    readback = D_8009B348->word;
    dma = (volatile u32 *)(0x1f801080 + (channel << 4));
    *D_8009B344 |= 1 << ((channel << 2) + 3);
    *dma++ = (u32)address;
    *dma++ = (blockCount << 16) | blockSize;
    while (!(*D_8009B32C & 0x40)) {}
    *dma = control;
    readback = *dma;
}
