#ifndef PE1_PSYQ_SPU_INTERNAL_H
#define PE1_PSYQ_SPU_INTERNAL_H

typedef signed int s32;
typedef unsigned int u32;
typedef unsigned long u_long;
typedef unsigned short u16;
typedef void (*SpuCallback)(void);

typedef struct SpuMalloc {
    u32 addr;
    u32 size;
} SPU_MALLOC;

extern s32 _spu_mem_mode_plus;
extern s32 _spu_mem_mode;
extern s32 _spu_mem_mode_unit;
extern s32 _spu_mem_mode_unitM;
extern unsigned char *_spu_RXX;
extern u32 *_spu_sys_pcr;
extern u32 *D_8009B410;
extern s32 D_8009B3EC;
extern s32 _spu_EVdma;
extern SpuCallback _spu_transferCallback;
extern SpuCallback _spu_IRQCallback;
extern int _spu_AllocBlockNum;
extern int _spu_AllocLastNum;
extern SPU_MALLOC *_spu_memList;

#define _spu_isCalled D_8009B3EC
#define _spu_delay D_8009B410

#endif
