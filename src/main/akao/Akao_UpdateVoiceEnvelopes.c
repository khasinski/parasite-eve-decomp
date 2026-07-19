
typedef signed char s8;
typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;
typedef int s32;
typedef unsigned int u32;
typedef long long s64;
void SpuGetVoiceEnvelope();
void Akao_RemoveVoice();
extern void *g_AkaoCurTrack[];
extern s16 g_AkaoVoiceEnvelopeTable[];
extern u8 g_AkaoVoiceStateTable[];
void Akao_UpdateVoiceEnvelopes(s32 arg0);
void Akao_UpdateVoiceEnvelopes(s32 arg0)
{
  u8 *envelope;
  s32 protected_mask;
  u32 voice_index;
  u32 mask;
  u8 *voice_base;
  voice_index = 0;
  mask = 1;
  voice_base = (u8 *) (&g_AkaoVoiceStateTable[0]);
  envelope = (u8 *) (&g_AkaoVoiceEnvelopeTable[0]);
  protected_mask = (((*((s32 *) (((u8 *) g_AkaoCurTrack[0]) + 4))) & (*((s32 *) (((u8 *) g_AkaoCurTrack[0]) + 0xC)))) | ((*((s32 *) (((u8 *) g_AkaoCurTrack[0]) + 0x6C))) & (*((s32 *) (((u8 *) g_AkaoCurTrack[0]) + 0x74))))) | arg0;
  do
  {
    if (protected_mask & (mask << voice_index))
    {
      register s32 temp_v0;
      temp_v0 = 0x7FFF;
      *((s16 *) envelope) = temp_v0;
    }
    else
    {
      SpuGetVoiceEnvelope(voice_index, envelope);
      if ((*((s16 *) envelope)) == 0)
      {
        if (envelope)
        {
          Akao_RemoveVoice(voice_base, voice_index);
          Akao_RemoveVoice(voice_base + 0x1AA0, voice_index);
        }
        else
        {
          Akao_RemoveVoice(voice_base, voice_index);
          Akao_RemoveVoice(voice_base + 0x1AA0, voice_index);
        }
      }
    }
    voice_index += 1;
    envelope += 8;
  }
  while (voice_index < 0x18U);
}
