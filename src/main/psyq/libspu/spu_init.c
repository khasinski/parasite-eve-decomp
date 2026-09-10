/* ASSEMBLER: GNU */
#include "pe1/psyq_spu_internal.h"
#include "pe1/psyq_bios.h"

int _spu_init(int hot) {
    unsigned int timeout;
    int channel;
    volatile u16 *clear;
    *_spu_sys_pcr |= 0xB0000;
    D_8009B418 = 0;
    D_8009B41C = 0;
    g_SpuTransferAddr = 0;
    _spu_RXX->master_volume_left = 0;
    _spu_RXX->master_volume_right = 0;
    _spu_RXX->spucnt = 0;
    _spu_Fw1ts();
    _spu_RXX->master_volume_left = 0;
    _spu_RXX->master_volume_right = 0;
    timeout = 0;
    while (_spu_RXX->transfer_status & 0x7FF) {
        if (++timeout > 0xF00) {
            printf(D_80011C4C, D_80011C5C);
            break;
        }
    }
    channel = 0;
    _spu_mem_mode = 2;
    _spu_mem_mode_plus = 3;
    _spu_mem_mode_unit = 8;
    _spu_mem_mode_unitM = 7;
    _spu_RXX->transfer_control = 4;
    _spu_RXX->reverb_volume_left = 0;
    _spu_RXX->reverb_volume_right = 0;
    _spu_RXX->key_off[0] = 0xFFFF;
    _spu_RXX->key_off[1] = 0xFFFF;
    _spu_RXX->reverb_enable[0] = 0;
    _spu_RXX->reverb_enable[1] = 0;
    clear = D_800B6900;
    /* Preserve the ascending halfword writes to the shared request buffer. */
    for (; channel < 10; channel++) {
        *clear++ = 0;
    }
    if (!hot) {
        g_SpuTransferAddr = 0x200;
        _spu_RXX->pitch_modulation[0] = 0;
        _spu_RXX->pitch_modulation[1] = 0;
        _spu_RXX->noise_enable[0] = 0;
        _spu_RXX->noise_enable[1] = 0;
        _spu_RXX->cd_volume_left = 0;
        _spu_RXX->cd_volume_right = 0;
        _spu_RXX->external_volume_left = 0;
        _spu_RXX->external_volume_right = 0;
        _spu_FwriteByIO(D_8009B43C, 0x10);
        for (channel = 0; channel < 24; channel++) {
            _spu_RXX->voice[channel].volume_left = 0;
            _spu_RXX->voice[channel].volume_right = 0;
            _spu_RXX->voice[channel].pitch = 0x3FFF;
            _spu_RXX->voice[channel].start_address = 0x200;
            _spu_RXX->voice[channel].adsr_low = 0;
            _spu_RXX->voice[channel].adsr_high = 0;
        }
        _spu_RXX->key_on[0] = 0xFFFF;
        _spu_RXX->key_on[1] = 0xFF;
        _spu_Fw1ts();
        _spu_Fw1ts();
        _spu_Fw1ts();
        _spu_Fw1ts();
        _spu_RXX->key_off[0] = 0xFFFF;
        _spu_RXX->key_off[1] = 0xFF;
        _spu_Fw1ts();
        _spu_Fw1ts();
        _spu_Fw1ts();
        _spu_Fw1ts();
    }
    D_8009B430 = 1;
    _spu_RXX->spucnt = 0xC000;
    _spu_transferCallback = 0;
    _spu_IRQCallback = 0;
    return 0;
}
