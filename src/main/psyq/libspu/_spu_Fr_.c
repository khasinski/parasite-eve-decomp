/* CC1_FLAGS: -g3 */
/* MASPSX_FLAGS: --stack-return-delay */
typedef unsigned short u16;
typedef unsigned int u32;

typedef struct SpuRegs {
    char pad0[0x1A6];
    volatile u16 trans_addr;
    char pad1[2];
    volatile u16 spucnt;
} SpuRegs;

extern SpuRegs *_spu_RXX;
extern volatile u32 *g_SpuDmaMadrPtr;
extern volatile u32 *g_SpuDmaBcrPtr;
extern volatile u32 *g_SpuDmaChcrPtr;
extern int g_SpuDmaDirection;

void _spu_Fw1ts(void);
void _spu_FsetDelayR(void);

void _spu_Fr_(int madr, u16 trans_addr, int bcr) {
    int shiftedBcr;

    _spu_RXX->trans_addr = trans_addr;
    _spu_Fw1ts();
    _spu_RXX->spucnt |= 0x30;
    _spu_Fw1ts();
    shiftedBcr = bcr << 16;
    _spu_FsetDelayR();
    *g_SpuDmaMadrPtr = madr;
    *g_SpuDmaBcrPtr = shiftedBcr | 0x10;
    g_SpuDmaDirection = 1;
    *g_SpuDmaChcrPtr = 0x01000200;
}
