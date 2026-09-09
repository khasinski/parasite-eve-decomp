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
typedef struct SpuVoiceRegs {
    volatile u16 volume_left;
    volatile u16 volume_right;
    volatile u16 pitch;
    volatile u16 start_address;
    volatile u16 adsr_low;
    volatile u16 adsr_high;
    volatile u16 envelope;
    volatile u16 repeat_address;
} SpuVoiceRegs;
typedef char SpuVoiceRegsSizeCheck[(sizeof(SpuVoiceRegs) == 0x10) ? 1 : -1];

typedef struct SpuRegs {
    /* 0x000 */ SpuVoiceRegs voice[24];
    /* 0x180 */ volatile u16 master_volume_left;
    /* 0x182 */ volatile u16 master_volume_right;
    /* 0x184 */ volatile u16 reverb_volume_left;
    /* 0x186 */ volatile u16 reverb_volume_right;
    /* 0x188 */ volatile u16 key_on[2];
    /* 0x18C */ volatile u16 key_off[2];
    /* 0x190 */ volatile u16 pitch_modulation[2];
    /* 0x194 */ volatile u16 noise_enable[2];
    /* 0x198 */ volatile u16 reverb_enable[2];
    /* 0x19C */ unsigned char pad_19C[0xA];
    /* 0x1A6 */ volatile u16 trans_addr;
    /* 0x1A8 */ volatile u16 transfer_fifo;
    /* 0x1AA */ volatile u16 spucnt;
    /* 0x1AC */ volatile u16 transfer_control;
    /* 0x1AE */ volatile u16 transfer_status;
    /* 0x1B0 */ volatile u16 cd_volume_left;
    /* 0x1B2 */ volatile u16 cd_volume_right;
    /* 0x1B4 */ volatile u16 external_volume_left;
    /* 0x1B6 */ volatile u16 external_volume_right;
    /* 0x1B8 */ volatile u16 current_master_volume_left;
    /* 0x1BA */ volatile u16 current_master_volume_right;
    /* 0x1BC */ unsigned char pad_1BC[4];
    /* 0x1C0 */ volatile u16 reverb[32];
} SpuRegs;
typedef char SpuRegsSizeCheck[(sizeof(SpuRegs) == 0x200) ? 1 : -1];

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
s32 _spu_Fr(void *address, s32 size);
u32 _spu_Fw(void *address, u32 size);
int _spu_init(int hot);
void _spu_FsetRXX(u32 offset, u32 value, u32 mode);
u32 _spu_FsetRXXa(s32 offset, u32 value);
u32 _spu_FgetRXXa(u32 offset, s32 mode);
long SpuSetNoiseClock(long clock);
extern int _spu_AllocBlockNum;
extern int _spu_AllocLastNum;
extern SPU_MALLOC *_spu_memList;

#define _spu_isCalled D_8009B3EC
#define _spu_delay D_8009B410

#endif
