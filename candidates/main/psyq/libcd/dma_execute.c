/* CC1_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */
extern int printf(const char *fmt, ...);

extern char D_80011C2C[];
extern volatile unsigned char *D_8009B348;
extern volatile unsigned int *D_8009B344;
extern volatile unsigned char *D_8009B32C;

void dma_execute(ch, madr, blocks, bsize, chcr, mode)
int ch;
unsigned int madr;
unsigned short blocks;
unsigned short bsize;
unsigned int chcr;
unsigned char mode;
{
    volatile unsigned int tmp;
    volatile unsigned int *reg;
    unsigned int enable;
    int i;

    for (i = 0; *(volatile unsigned int *)(0x1F801088 + (ch << 4)) & 0x01000000; i++) {
        if (i == 0x10000) {
            printf(D_80011C2C, *(volatile unsigned int *)(0x1F801088 + (ch << 4)));
            break;
        }
    }

    if (mode == 1) {
        D_8009B348[2] = D_8009B348[2] | (1 << ch);
    } else {
        D_8009B348[2] = D_8009B348[2] & ~(1 << ch);
    }

    tmp = *(volatile unsigned int *)D_8009B348;

    enable = 1 << ((ch << 2) + 3);
    reg = (volatile unsigned int *)(0x1F801080 + (ch << 4));
    *D_8009B344 = *D_8009B344 | enable;
    *reg = madr;
    reg++;
    *reg = (blocks << 16) | bsize;

    while (!(*D_8009B32C & 0x40)) {
    }

    reg++;
    *reg = chcr;
    tmp = *reg;
}
