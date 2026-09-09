#include "pe1/akao.h"

extern AkaoTrack g_AkaoVoiceStateTable[];

void Akao_WriteVoiceParam();

void Spu_CopyVoiceToStereoSlot(AkaoTrack *track, int stereo_voice_index) {
    int left;
    int right;
    int pan;
    int new_var;
    register int inverse_pan;
    AkaoTrack *stereo;

    left = track->volume_left;
    inverse_pan = 0x7F;
    pan = (unsigned short)track->pan_target;
    pan = ((int)(pan << 16)) >> 24;
    inverse_pan -= pan;
    track->volume_left = ((unsigned int)(left * inverse_pan)) >> 8;
    stereo = &g_AkaoVoiceStateTable[stereo_voice_index];
    new_var = inverse_pan;
    stereo->volume_left = (left * ((short)track->pan_target)) >> 16;
    right = (left = track->volume_right);
    track->volume_right = ((unsigned int)(right * new_var)) >> 8;
    stereo->volume_right = (right * ((short)track->pan_target)) >> 16;
    stereo->pitch = track->pitch;
    stereo->update_flags |= (*track).update_flags;
    Akao_WriteVoiceParam(track->assigned_voice_index, (AkaoVoiceParams *)(&track->assigned_voice_index), track->flags);
    Akao_WriteVoiceParam(stereo_voice_index, (AkaoVoiceParams *)(&stereo->assigned_voice_index), track->flags);
}
#include "pe1/akao.h"

typedef unsigned int u32;

void Akao_WriteVoiceParam();

extern AkaoTrack g_AkaoVoiceStateTable[];

void Spu_RestoreVoiceFromStereoSlot(AkaoTrack *track, int stereo_voice_index) {
    AkaoTrack *track_reg;
    int index_reg;
    AkaoVoiceParams *params;
    register int mask asm("$7");
    int idx_arg;
    int flags_arg;
    AkaoVoiceParams *call_params;
    int index;
    register u32 left asm("$3");
    u32 right;
    register u32 flags asm("$3");
    AkaoTrack *stereo;

    track_reg = track;
    index_reg = stereo_voice_index;
    params = &AKAO_TRACK_VOICE(track_reg);
    Akao_WriteVoiceParam(track_reg->assigned_voice_index, params, track_reg->flags);
    mask = 0x1FF93;
    idx_arg = index_reg;
    asm volatile("" : "=r"(idx_arg) : "0"(idx_arg));
    index = idx_arg;
    index = index << 3;
    index += idx_arg;
    index = index << 3;
    index -= idx_arg;
    index = index << 2;
    stereo = (AkaoTrack *)((int)&g_AkaoVoiceStateTable + index);
    flags_arg = track_reg->flags;
    asm volatile("" : "=r"(flags_arg) : "0"(flags_arg));
    left = (unsigned short)stereo->volume_left;
    call_params = params;
    track_reg->volume_left = left;
    flags = track_reg->update_flags;
    right = (unsigned short)stereo->volume_right;
    flags = flags | mask;
    track_reg->update_flags = flags;
    track_reg->volume_right = right;
    Akao_WriteVoiceParam(idx_arg, call_params, flags_arg, mask);
}
