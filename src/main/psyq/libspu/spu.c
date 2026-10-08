/* ASSEMBLER: GNU */
/* PSY-Q LIBSPU SPU, part 1 of 5: _spu_init, _spu_FwriteByIO. */
/* SPU stays in five units: spu.c and spu_2.c only match with GCC 2.7.2,
 * spu_3.c and spu_5.c only with GCC 2.8.1, spu_4.c only with the GAS 2.8.1
 * assembler. */
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
    _spu_mem_mode.mode = 2;
    _spu_mem_mode.shift = 3;
    _spu_mem_mode.bytes = 8;
    _spu_mem_mode.mask = 7;
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

extern char D_80011C4C[], D_80011C6C[], D_80011C80[];
int printf(const char *, ...);

void _spu_FwriteByIO(void *address, u32 size) {
    u16 *source = address;
    u16 initial_status;
    u32 timer;
    int count, i;
    u16 control;
    {
        SpuRegs *spu = _spu_RXX;
        initial_status = spu->transfer_status & 0x7FF;
        spu->trans_addr = g_SpuTransferAddr;
    }
    _spu_Fw1ts();
    while (size) {
        count = size > 64 ? 64 : size;
        i = 0;
        if (count > 0) {
            do {
                _spu_RXX->transfer_fifo = *source++;
                i += 2;
            } while (i < count);
        }
        {
            SpuRegs *spu = _spu_RXX;
            control = spu->spucnt;
            /* Single control write in the following wait call's delay slot. */
            *(u16 *)&spu->spucnt = (control & ~0x30) | 0x10;
        }
        _spu_Fw1ts();
        timer = 0;
        while (_spu_RXX->transfer_status & 0x400) {
            if (++timer > 0xF00u) {
                printf(D_80011C4C, D_80011C6C);
                break;
            }
        }
        _spu_Fw1ts();
        size -= count;
        _spu_Fw1ts();
    }
    {
        SpuRegs *spu = _spu_RXX;
        control = spu->spucnt;
        spu->spucnt = control & ~0x30;
    }
    timer = 0;
    while ((_spu_RXX->transfer_status & 0x7FF) != initial_status) {
        if (++timer > 0xF00u) {
            printf(D_80011C4C, D_80011C80);
            break;
        }
    }
}
