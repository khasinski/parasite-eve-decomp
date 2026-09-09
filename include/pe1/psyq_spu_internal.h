#ifndef PE1_PSYQ_SPU_INTERNAL_H
#define PE1_PSYQ_SPU_INTERNAL_H

#ifndef PE1_COMMON_H
typedef signed int s32;
typedef unsigned int u32;
typedef unsigned short u16;
#endif
typedef unsigned long u_long;
typedef void (*SpuCallback)(void);

/*
 * SPU_RXX register map from Psy-Q libspu.  The three transfer registers are
 * at the same offsets as the SDK's trans_addr, spucnt, and spustat fields.
 */
typedef struct SpuRegs {
    /* 0x000 */ unsigned char pad_000[0x184];
    /* 0x184 */ volatile u16 master_volume_left;
    /* 0x186 */ volatile u16 master_volume_right;
    /* 0x188 */ unsigned char pad_188[0x1E];
    /* 0x1A6 */ volatile u16 trans_addr;
    /* 0x1A8 */ volatile u16 transfer_fifo;
    /* 0x1AA */ volatile u16 spucnt;
    /* 0x1AC */ unsigned char pad_1AC[2];
    /* 0x1AE */ volatile u16 transfer_status;
} SpuRegs;

typedef struct SpuMalloc {
    u32 addr;
    u32 size;
} SPU_MALLOC;

extern s32 _spu_mem_mode_plus;
extern s32 _spu_mem_mode;
extern s32 _spu_mem_mode_unit;
extern s32 _spu_mem_mode_unitM;
extern SpuRegs *_spu_RXX;
extern u32 *_spu_sys_pcr;
extern u32 *D_8009B410;
extern s32 D_8009B3EC;
extern s32 _spu_EVdma;
/* Shared by transfer callers and the DMA interrupt handler. */
extern SpuCallback volatile _spu_transferCallback;
SpuCallback SpuSetTransferCallback(SpuCallback callback);
extern SpuCallback _spu_IRQCallback;
SpuCallback Spu_SetTransferMode(SpuCallback callback);
void _SpuCallback(SpuCallback callback);
void _SpuDataCallback(SpuCallback callback);
extern int _spu_AllocBlockNum;
extern int _spu_AllocLastNum;
extern SPU_MALLOC *_spu_memList;

#define _spu_isCalled D_8009B3EC
#define _spu_delay D_8009B410

#endif
