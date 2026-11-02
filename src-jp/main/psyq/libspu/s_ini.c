/* ASSEMBLER: GNU */
/* PSY-Q LIBSPU S_INI: _SpuInit, SpuStart. */
/* Complete LIBSPU S_INI.OBJ code; object alignment stays in the manifest. */

#include "pe1/psyq_bios.h"
#include "pe1/psyq_callbacks.h"
#include "pe1/psyq_spu_internal.h"

#include "s_ini_externs.h"

void _SpuInit(int mode) {
    int i;
    int reverbRegister;
    SpuReverbState *reverb;
    ResetCallback();
    _spu_init(mode);
    if (!mode) {
        unsigned short value = 0xC000;
        unsigned short *base = _spu_voice_centerNote;
        i = 23;
        do {
            base[i] = value;
            --i;
        } while (i >= 0);
    }
    SpuStart();
    /* Keep the register selector and shared reverb base ahead of the resets. */
    reverbRegister = 0xD1;
    reverb = &g_SpuReverbState;
    asm volatile("" : "=r"(reverbRegister), "=r"(reverb)
        : "0"(reverbRegister), "1"(reverb));
    _spu_rev_flag = 0;
    _spu_rev_reserve_wa = 0;
    reverb->mode = 0;
    reverb->depth.left = 0;
    reverb->depth.right = 0;
    reverb->delay = 0;
    reverb->feedback = 0;
    _spu_rev_offsetaddr = D_8009AF34;
    _spu_FsetRXX(reverbRegister, _spu_rev_offsetaddr, 0);
    _spu_AllocBlockNum = 0;
    _spu_AllocLastNum = 0;
    _spu_memList = 0;
    _spu_trans_mode = 0;
    _spu_transMode = 0;
    _spu_keystat = 0;
    _spu_RQmask = 0;
    _spu_RQvoice = 0;
    _spu_env = 0;
}

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
