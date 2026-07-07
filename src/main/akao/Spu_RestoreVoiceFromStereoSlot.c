#include "pe1/akao.h"

typedef unsigned int u32;

void Akao_WriteVoiceParam();

extern AkaoTrack g_AkaoVoiceStateTable[];

void Spu_RestoreVoiceFromStereoSlot(AkaoTrack *track, int stereo_voice_index) {
    AkaoTrack *track_reg;
    int index_reg;
    AkaoVoiceParams *params;
    register int mask asm("$7");
    register int idx_arg asm("$4");
    register int flags_arg asm("$6");
    AkaoVoiceParams *call_params;
    int index;
    register u32 left asm("$3");
    register u32 right asm("$2");
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
