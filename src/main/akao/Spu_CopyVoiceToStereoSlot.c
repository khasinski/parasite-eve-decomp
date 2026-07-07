#include "pe1/akao.h"

void Akao_WriteVoiceParam();

extern AkaoTrack g_AkaoVoiceStateTable[];

void Spu_CopyVoiceToStereoSlot(AkaoTrack *track, int stereo_voice_index) {
    int left;
    int right;
    int pan;
    register int inverse_pan asm("$3");
    AkaoTrack *stereo;

    left = track->volume_left;
    inverse_pan = 0x7F;
    pan = (unsigned short)track->pan_target;
    pan = (int)(pan << 16) >> 24;
    inverse_pan -= pan;
    track->volume_left = (unsigned int)(left * inverse_pan) >> 8;
    stereo = &g_AkaoVoiceStateTable[stereo_voice_index];
    stereo->volume_left = (left * (short)track->pan_target) >> 16;
    right = track->volume_right;
    track->volume_right = (unsigned int)(right * inverse_pan) >> 8;
    stereo->volume_right = (right * (short)track->pan_target) >> 16;
    stereo->pitch = track->pitch;
    stereo->update_flags |= track->update_flags;
    Akao_WriteVoiceParam(track->assigned_voice_index, (AkaoVoiceParams *)&track->assigned_voice_index, track->flags);
    Akao_WriteVoiceParam(stereo_voice_index, (AkaoVoiceParams *)&stereo->assigned_voice_index, track->flags);
}
