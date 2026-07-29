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
