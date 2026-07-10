/* MASPSX_FLAGS: --store-return-delay --stack-return-delay */

typedef unsigned char u8;
typedef unsigned int u32;

extern volatile u32 *D_8009B344;
extern volatile u8 *D_8009B348;
extern volatile u8 *D_8009B32C;
extern char D_80011C2C[];

int printf(char *fmt, int arg);

void dma_execute(int chan, u32 madr, u32 bcr_hi, u32 bcr_lo, u32 chcr, u8 enable) {
    register int chan_reg asm("$16") = chan;
    register u32 madr_reg asm("$18") = madr;
    register u32 bcr_hi_reg asm("$19") = bcr_hi;
    register u32 bcr_lo_reg asm("$20") = bcr_lo;
    register int enable_reg asm("$17") = enable;
    register u32 offset asm("$5");
    register volatile u32 *dma_reg asm("$5");
    register volatile u8 *intr_reg asm("$3");
    register u32 value asm("$2");
    register u32 bit asm("$3");
    register u32 counter asm("$4");
    register u32 chcr_late asm("$2");
    volatile u32 scratch;

    counter = 0;
    offset = chan_reg << 4;
    if (*(volatile u32 *)(0x1F801088 + offset) & 0x1000000) {
        do {
            if (counter == 0x10000) {
                printf(D_80011C2C, *(volatile u32 *)(0x1F801088 + offset));
                break;
            }
            counter++;
        } while (*(volatile u32 *)(0x1F801088 + offset) & 0x1000000);
    }

    value = 1;
    intr_reg = D_8009B348;
    if (enable_reg == value) {
        value = intr_reg[2] | (value << chan_reg);
    } else {
        value = intr_reg[2] & ~(value << chan_reg);
    }
    intr_reg[2] = value;

    scratch = *(u32 *)D_8009B348;

    bit = 1 << ((chan_reg << 2) + 3);
    dma_reg = (volatile u32 *)(0x1F801080 + offset);
    *D_8009B344 |= bit;
    dma_reg[0] = madr_reg;
    dma_reg[1] = (bcr_hi_reg << 16) | bcr_lo_reg;

    intr_reg = D_8009B32C;
    if ((intr_reg[0] & 0x40) == 0) {
        while ((intr_reg[0] & 0x40) == 0) {
        }
    }

    asm volatile("lw %0, 0x40($sp)" : "=r"(chcr_late));
    dma_reg[2] = chcr_late;
    value = dma_reg[2];
    scratch = value;
}
