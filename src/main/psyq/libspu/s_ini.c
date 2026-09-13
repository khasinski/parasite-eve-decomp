/* ASSEMBLER: GNU */
/* Complete LIBSPU S_INI.OBJ code; object alignment stays in the manifest. */
#include "pe1/psyq_bios.h"
#include "pe1/psyq_callbacks.h"
#include "pe1/psyq_spu_internal.h"

void _SpuInit(int mode) {
    int i;
    int reverbRegister;
    SpuReverbState *reverb;
    ResetCallback();
    _spu_init(mode);
    if (!mode) {
        unsigned short value = 0xC000;
        unsigned short *base = D_8009B3B8;
        i = 23;
        do {
            base[i] = value;
            --i;
        } while (i >= 0);
    }
    SpuStart();
    /* Keep the register selector and shared reverb base ahead of the resets. */
    reverbRegister = 0xD1;
    asm volatile("" : "=r"(reverbRegister) : "0"(reverbRegister));
    reverb = &D_8009B3A0;
    asm volatile("" : "=r"(reverb) : "0"(reverb));
    _spu_rev_flag = 0;
    _spu_rev_reserve_wa = 0;
    reverb->mode = 0;
    reverb->depth[0] = 0;
    reverb->depth[1] = 0;
    reverb->delay = 0;
    reverb->feedback = 0;
    _spu_rev_offsetaddr = D_8009B46C;
    _spu_FsetRXX(reverbRegister, _spu_rev_offsetaddr, 0);
    D_8009B45C = 0;
    D_8009B460 = 0;
    D_8009B464 = 0;
    D_8009B38C = 0;
    D_8009B418 = 0;
    D_8009B388 = 0;
    D_8009B3B4 = 0;
    D_8009B3B0 = 0;
    D_8009B3E8 = 0;
}

extern s32 OpenEvent(u32 event, s32 spec, s32 mode, s32 (*callback)(void));
extern s32 EnableEvent(s32 event);

void SpuStart(void) {
    if (_spu_isCalled == 0) {
        _spu_isCalled = 1;
        EnterCriticalSection();
        _SpuDataCallback(_spu_FiDMA);
        _spu_EVdma = OpenEvent(0xF0000009, 0x20, 0x2000, 0);
        EnableEvent(_spu_EVdma);
        ExitCriticalSection();
    }
}