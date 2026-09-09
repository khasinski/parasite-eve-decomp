#include "pe1/psyq_spu_internal.h"
extern u16 g_SpuTransferAddr;
extern int D_8009B418, D_8009B41C, g_SpuTransferActiveFlag;
extern u16 D_800B6900[10];
extern unsigned char D_8009B43C[16];
extern char D_80011C4C[], D_80011C5C[];
void _spu_Fw1ts(void);
void _spu_FwriteByIO(void *address, u32 size);
int printf(const char *, ...);

int _spu_init(int hot) {
    u32 timer;
    int i;
    *_spu_sys_pcr |= 0xB0000;
    {
        SpuRegs *spu = _spu_RXX;
        D_8009B418 = 0;
        D_8009B41C = 0;
        g_SpuTransferAddr = 0;
        spu->master_volume_left = 0;
        spu->master_volume_right = 0;
        spu->spucnt = 0;
    }
    _spu_Fw1ts();
    {
        SpuRegs *spu = _spu_RXX;
        spu->master_volume_left = 0;
        spu->master_volume_right = 0;
    }
    timer = 0;
    while (_spu_RXX->transfer_status & 0x7FF) {
        if (++timer > 0xF00u) {
            printf(D_80011C4C, D_80011C5C);
            break;
        }
    }
    _spu_mem_mode = 2;
    _spu_mem_mode_plus = 3;
    _spu_mem_mode_unit = 8;
    _spu_mem_mode_unitM = 7;
    {
        SpuRegs *spu = _spu_RXX;
        spu->transfer_control = 4;
        spu->reverb_volume_left = 0;
        spu->reverb_volume_right = 0;
        spu->key_off[0] = 0xFFFF;
        spu->key_off[1] = 0xFFFF;
        spu->reverb_enable[0] = 0;
        spu->reverb_enable[1] = 0;
    }
    for (i = 0; i < 10; i++) D_800B6900[i] = 0;
    if (!hot) {
        SpuVoiceRegs *voice;
        {
            SpuRegs *spu = _spu_RXX;
            g_SpuTransferAddr = 0x200;
            spu->pitch_modulation[0] = 0;
            spu->pitch_modulation[1] = 0;
            spu->noise_enable[0] = 0;
            spu->noise_enable[1] = 0;
            spu->cd_volume_left = 0;
            spu->cd_volume_right = 0;
            spu->external_volume_left = 0;
            spu->external_volume_right = 0;
        }
        _spu_FwriteByIO(D_8009B43C, 16);
        voice = _spu_RXX->voice;
        for (i = 0; i < 24; i++, voice++) {
            voice->volume_left = 0;
            voice->volume_right = 0;
            voice->pitch = 0x3FFF;
            voice->start_address = 0x200;
            voice->adsr_low = 0;
            voice->adsr_high = 0;
        }
        {
            SpuRegs *spu = _spu_RXX;
            spu->key_on[0] = 0xFFFF;
            spu->key_on[1] = 0xFF;
        }
        _spu_Fw1ts(); _spu_Fw1ts(); _spu_Fw1ts(); _spu_Fw1ts();
        {
            SpuRegs *spu = _spu_RXX;
            spu->key_off[0] = 0xFFFF;
            spu->key_off[1] = 0xFF;
        }
        _spu_Fw1ts(); _spu_Fw1ts(); _spu_Fw1ts(); _spu_Fw1ts();
    }
    {
        SpuRegs *spu = _spu_RXX;
        g_SpuTransferActiveFlag = 1;
        spu->spucnt = 0xC000;
        _spu_transferCallback = 0;
        _spu_IRQCallback = 0;
    }
    return 0;
}
