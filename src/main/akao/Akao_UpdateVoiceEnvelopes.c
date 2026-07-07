/* CC1_FLAGS: -g3 -G8 */
/* MASPSX_FLAGS: -G8 */

typedef signed char s8;
typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;
typedef int s32;
typedef unsigned int u32;
typedef long long s64;

#define NULL ((void *)0)

void SpuGetVoiceEnvelope();
void Akao_RemoveVoice();

extern void * g_AkaoCurTrack[];
#define g_AkaoCurTrack (g_AkaoCurTrack[0])
extern s16 g_AkaoVoiceEnvelopeTable[];
#define g_AkaoVoiceEnvelopeTable (g_AkaoVoiceEnvelopeTable[0])
extern u8 g_AkaoVoiceStateTable[];
#define g_AkaoVoiceStateTable (g_AkaoVoiceStateTable[0])

#define S32_AT(ptr, offset) (*(s32 *)((u8 *)(ptr) + (offset)))

void Akao_UpdateVoiceEnvelopes(s32 arg0);

void Akao_UpdateVoiceEnvelopes(s32 arg0) {
    u8 *envelope;
    s32 protected_mask;
    u32 voice_index;
    u32 mask;
    u8 *voice_base;

    voice_index = 0;
    mask = 1;
    voice_base = (u8 *) &g_AkaoVoiceStateTable;
    envelope = (u8 *) &g_AkaoVoiceEnvelopeTable;
    protected_mask = (S32_AT(g_AkaoCurTrack, 4) & S32_AT(g_AkaoCurTrack, 0xC)) | (S32_AT(g_AkaoCurTrack, 0x6C) & S32_AT(g_AkaoCurTrack, 0x74)) | arg0;
    do {
        if (protected_mask & (mask << voice_index)) {
            register s32 temp_v0 asm("$2");
            temp_v0 = 0x7FFF;
            *(s16 *)envelope = temp_v0;
        } else {
            SpuGetVoiceEnvelope(voice_index, envelope);
            if (*(s16 *)envelope == 0) {
                Akao_RemoveVoice(voice_base, voice_index);
                Akao_RemoveVoice(voice_base + 0x1AA0, voice_index);
            }
        }
        voice_index += 1;
        envelope += 8;
    } while (voice_index < 0x18U);
}
