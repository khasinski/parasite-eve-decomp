typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
extern char *g_AkaoCurTrack;
extern u32 g_AkaoVoiceUpdateFlags;
extern u16 g_AkaoTrack5ATransposeValue;

#include "pe1/akao.h"

unsigned int SeqOp_SetTrack5AValue(void *ptr)
{
  u8 *pc;
  int value;
  register char *track;
  pc = *((u8 **) ptr);
  *((u8 **) ptr) = pc + 1;
  value = pc[0];
  if ((*((u16 *) (((char *) ptr) + 0x54))) == 0)
  {
    if ((value & 0xC0) != 0)
    {
      ;
      *((u16 *) (g_AkaoCurTrack + 0x5A)) = ((*((u16 *) (g_AkaoCurTrack + 0x5A))) + (value & 0x3F)) & 0x3F;
    }
    else
    {
      track = g_AkaoCurTrack;
      *((u16 *) (track + 0x5A)) = value;
    }
  }
  else
    if ((value & 0xC0) != 0)
  {
    int m = value & 0x3F;
    g_AkaoTrack5ATransposeValue = (g_AkaoTrack5ATransposeValue + m) & 0x3F;
  }
  else
  {
    g_AkaoTrack5ATransposeValue = value;
  }
  g_AkaoVoiceUpdateFlags |= 0x10;
}

void sndTrackReadAdsrAttackRate(AkaoTrack *track) {
    unsigned char *cursor = track->pc;
    unsigned int flags;
    unsigned char value;

    track->pc = cursor + 1;
    flags = track->update_flags;
    value = *cursor;
    track->update_flags = flags | 0x900;
    track->adsr_attack_rate = value;
}

void sndTrackReadAdsrDecayRate(AkaoTrack *track) {
    unsigned char *cursor = track->pc;
    unsigned int flags;
    unsigned char value;

    track->pc = cursor + 1;
    flags = track->update_flags;
    value = *cursor;
    track->update_flags = flags | 0x1000;
    track->adsr_decay_rate = value;
}

void sndTrackReadAdsrSustainLevel(AkaoTrack *track) {
    unsigned char *cursor = track->pc;
    unsigned int flags;
    unsigned char value;

    track->pc = cursor + 1;
    flags = track->update_flags;
    value = *cursor;
    track->update_flags = flags | 0x8000;
    track->adsr_sustain_level = value;
}

void sndTrackReadAdsrSustainRate(AkaoTrack *track) {
    unsigned char *cursor = track->pc;
    unsigned int flags;
    unsigned char value;

    track->pc = cursor + 1;
    flags = track->update_flags;
    value = *cursor;
    track->update_flags = flags | 0x2200;
    track->adsr_sustain_rate = value;
}

void sndTrackReadAdsrReleaseRate(AkaoTrack *track) {
    unsigned char *cursor = track->pc;
    unsigned int flags;
    unsigned char value;

    track->pc = cursor + 1;
    flags = track->update_flags;
    value = *cursor;
    track->update_flags = flags | 0x4400;
    track->adsr_release_rate = value;
}

void sndTrackReadAdsrAttack(AkaoTrack *track) {
    unsigned char *cursor = track->pc;
    unsigned int flags;
    unsigned char value;

    track->pc = cursor + 1;
    flags = track->update_flags;
    value = *cursor;
    track->update_flags = flags | 0x100;
    track->adsr_attack = value;
}

void sndTrackReadAdsrSustain(AkaoTrack *track) {
    unsigned char *cursor = track->pc;
    unsigned int flags;
    unsigned char value;

    track->pc = cursor + 1;
    flags = track->update_flags;
    value = *cursor;
    track->update_flags = flags | 0x200;
    track->adsr_sustain = value;
}

void sndTrackReadAdsrRelease(AkaoTrack *track) {
    unsigned char *cursor = track->pc;
    unsigned int flags;
    unsigned char value;

    track->pc = cursor + 1;
    flags = track->update_flags;
    value = *cursor;
    track->update_flags = flags | 0x400;
    track->adsr_release = value;
}

void Akao_SetReleaseRate(AkaoTrack *track) {
    unsigned char *cursor;
    unsigned int flags;
    unsigned char value;

    track->flags |= 0x200;
    cursor = track->pc;
    track->pc = cursor + 1;
    flags = track->update_flags;
    value = *cursor;
    track->update_flags = flags | 0x4400;
    track->adsr_release_rate = value;
}
