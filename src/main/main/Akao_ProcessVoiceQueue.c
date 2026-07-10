typedef signed short s16;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned char u8;

#define U16(base, off) (*(u16 *)((u8 *)(base) + (off)))
#define S16(base, off) (*(s16 *)((u8 *)(base) + (off)))
#define U32(base, off) (*(u32 *)((u8 *)(base) + (off)))

extern u32 D_800BCD50;
extern u32 D_800BCD54;
extern u32 D_800BCD58;
extern u32 D_800BCD60;
extern u16 D_800BCD78;
extern u32 D_8009D2C4;
extern u32 *D_8009D2C8;
extern u8 D_800BA560[];
extern u8 D_800B8AC0[];
extern u8 D_800BC000[];
extern u32 D_800C0DD0;
extern u32 D_800C0DD4;
extern u32 D_800C0DD8;

void Akao_UpdateVoiceEnvelopes(u32 blocked_mask);
void Akao_StepVoiceNote(void *voices, u32 active_mask, u32 restart_mask, u32 *key_on_mask);
void Akao_SetVoiceKeyOn(void *voice, u32 voice_mask);
void Akao_WriteVoiceParam(u32 mode, void *voice_param, u32 flags);
void Akao_SetMasterVolume(s16 left, s16 right);
void Spu_SetGlobalVolumeField1AA(u32 value);
void Akao_SetVoiceAdsr(void);
void Akao_SetVoiceVolume(void);
void Akao_SetVoiceStartAddr(void);
void Spu_WriteReverbEnable(u32 mask);
void Spu_WriteNoiseEnable(u32 mask);
void Spu_WriteFmEnable(u32 mask);
void Spu_WriteKeyOn(u32 mask);

void Akao_ProcessVoiceQueue(void) {
    u32 key_on_mask;
    u32 blocked_mask;
    u32 secondary_pending;
    u32 secondary_restart;
    u32 primary_pending;
    u32 primary_restart;
    u32 flags;
    u32 *state;
    u32 bit;
    u8 *voice;
    u8 *voice_tail;

    key_on_mask = 0;
    blocked_mask = D_800BCD50 | D_800BCD60;

    state = D_8009D2C8;
    if (((state[1] & state[4]) | (state[0x1B] & state[0x1E])) != 0) {
        Akao_UpdateVoiceEnvelopes(blocked_mask);
    }

    state = D_8009D2C8;
    secondary_pending = (state[0x1B] & state[0x1F]) & ~(state[0x1D] & blocked_mask);
    secondary_restart = (secondary_pending & state[0x1D]) & ~blocked_mask;
    if ((secondary_pending & state[0x1C]) != 0) {
        D_8009D2C8 = (u32 *)((u8 *)state + 0x68);
        Akao_StepVoiceNote(D_800BA560, secondary_pending & state[0x1C], secondary_restart, &key_on_mask);

        state = D_8009D2C8;
        D_8009D2C8 = (u32 *)((u8 *)state - 0x68);
        secondary_pending &= ~state[2];
        state[4] &= ~state[2];
    }

    state = D_8009D2C8;
    primary_pending = (state[1] & state[5]) & ~(state[3] & (secondary_restart | blocked_mask));
    primary_restart = (primary_pending & state[3]) & ~(secondary_restart | blocked_mask);
    if ((primary_pending & state[2]) != 0) {
        Akao_StepVoiceNote(D_800B8AC0, primary_pending & state[2], primary_restart, &key_on_mask);

        state = D_8009D2C8;
        primary_pending &= ~state[2];
        state[4] &= ~state[2];
    }

    if (secondary_pending != 0) {
        state = D_8009D2C8;
        D_8009D2C8 = (u32 *)((u8 *)state + 0x68);
        Akao_StepVoiceNote(D_800BA560, secondary_pending, secondary_restart & ~primary_restart, &key_on_mask);
        D_8009D2C8[4] = 0;
        D_8009D2C8 = (u32 *)((u8 *)D_8009D2C8 - 0x68);
    }

    if (primary_pending != 0) {
        Akao_StepVoiceNote(D_800B8AC0, primary_pending, primary_restart, &key_on_mask);
        D_8009D2C8[4] = 0;
    }

    primary_pending = D_800BCD50 & D_800BCD58;
    if (primary_pending != 0) {
        bit = 0x1000;
        voice = D_800BC000;
        voice_tail = voice + 0x38;
        key_on_mask |= D_800BCD54;

        while (primary_pending != 0) {
            if ((primary_pending & bit) != 0) {
                Akao_SetVoiceKeyOn(voice, bit);
                if (U32(voice_tail, 0xBC) != 0) {
                    Akao_WriteVoiceParam(U32(voice_tail, 0xB8), voice + 0xF0, U32(voice_tail, 0));
                }
                primary_pending &= ~bit;
            }
            bit <<= 1;
            voice_tail += 0x11C;
            voice += 0x11C;
        }

        D_800BCD54 = 0;
    }

    flags = D_8009D2C4;
    if ((flags & 0x80) != 0) {
        Akao_SetMasterVolume(S16(D_8009D2C8, 0x42), S16(D_8009D2C8, 0x42));
        D_8009D2C4 &= ~0x80;
    }

    if ((flags & 0x10) != 0) {
        if (D_800BCD50 != 0) {
            Spu_SetGlobalVolumeField1AA(D_800BCD78);
        } else {
            Spu_SetGlobalVolumeField1AA(U16(D_8009D2C8, 0x5A));
        }
        D_8009D2C4 &= ~0x10;
    }

    if ((flags & 0x100) != 0) {
        Akao_SetVoiceAdsr();
        Akao_SetVoiceVolume();
        Akao_SetVoiceStartAddr();
        Spu_WriteReverbEnable(D_800C0DD0);
        Spu_WriteNoiseEnable(D_800C0DD4);
        Spu_WriteFmEnable(D_800C0DD8);
        D_8009D2C4 &= ~0x100;
    }

    if (key_on_mask != 0) {
        Spu_WriteKeyOn(key_on_mask);
    }
}
