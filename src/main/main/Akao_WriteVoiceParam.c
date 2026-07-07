/* CC1_FLAGS: -g3 */
#include "pe1/akao.h"

void Akao_WriteVoiceParam(int voice_index, AkaoVoiceParams *params)
{
    unsigned int flags;
    unsigned int cur;

    flags = params->flags;
    if (flags != 0) {
        if (flags & AKAO_VOICE_PARAM_PITCH) {
            AkaoSpuVoice_SetPitch(voice_index, params->pitch);
            {
                register unsigned int mask asm("$3") = ~AKAO_VOICE_PARAM_PITCH;
                cur = params->flags & mask;
            }
            params->flags = cur;
            if (cur == 0) {
                return;
            }
        }
        if (flags & AKAO_VOICE_PARAM_VOLUME) {
            AkaoSpuVoice_SetVolume(voice_index, params->volume_left, params->volume_right);
            {
                register unsigned int mask asm("$3") = ~AKAO_VOICE_PARAM_VOLUME;
                cur = params->flags & mask;
            }
            params->flags = cur;
            if (cur == 0) {
                return;
            }
        }
        if (flags & AKAO_VOICE_PARAM_START_ADDR) {
            AkaoSpuVoice_SetStartAddress(voice_index, params->start_address);
            {
                register unsigned int mask asm("$3") = ~AKAO_VOICE_PARAM_START_ADDR;
                cur = params->flags & mask;
            }
            params->flags = cur;
            if (cur == 0) {
                return;
            }
        }
        if (flags & AKAO_VOICE_PARAM_LOOP_ADDR) {
            AkaoSpuVoice_SetRepeatAddress(voice_index, params->loop_address);
            cur = params->flags & ~AKAO_VOICE_PARAM_LOOP_ADDR;
            params->flags = cur;
            if (cur == 0) {
                return;
            }
        }
        if (flags & AKAO_VOICE_PARAM_ADSR_SUSTAIN) {
            AkaoSpuVoice_SetAdsrSustainRate(voice_index, params->adsr_sustain_rate, params->adsr_sustain_mode);
            {
                register unsigned int mask asm("$3") = ~AKAO_VOICE_PARAM_ADSR_SUSTAIN;
                cur = params->flags & mask;
            }
            params->flags = cur;
            if (cur == 0) {
                return;
            }
        }
        if (flags & AKAO_VOICE_PARAM_ADSR_ATTACK) {
            AkaoSpuVoice_SetAdsrAttack(voice_index, params->adsr_attack_rate, params->adsr_attack_mode);
            {
                register unsigned int mask asm("$3") = ~AKAO_VOICE_PARAM_ADSR_ATTACK;
                cur = params->flags & mask;
            }
            params->flags = cur;
            if (cur == 0) {
                return;
            }
        }
        if (flags & AKAO_VOICE_PARAM_ADSR_RELEASE) {
            AkaoSpuVoice_SetAdsrReleaseRate(voice_index, params->adsr_release_rate, params->adsr_release_mode);
            {
                register unsigned int mask asm("$3") = ~AKAO_VOICE_PARAM_ADSR_RELEASE;
                cur = params->flags & mask;
            }
            params->flags = cur;
            if (cur == 0) {
                return;
            }
        }
        if (flags & AKAO_VOICE_PARAM_ADSR_DECAY_SUSTAIN) {
            AkaoSpuVoice_SetAdsrDecayRate(voice_index, params->adsr_decay_rate);
            AkaoSpuVoice_SetAdsrSustainLevel(voice_index, params->adsr_sustain_level);
        }
        params->flags = 0;
    }
}
