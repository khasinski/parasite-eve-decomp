#include "common.h"
#include "pe1/akao.h"

extern char g_AkaoTrackStateBackup[];
extern AkaoVoiceBank g_AkaoVoiceStateBackup[];
extern AkaoVoiceBank g_AkaoVoiceBanks[] __asm__("g_AkaoVoiceStateTable");
extern char *g_AkaoCurTrack;

void Util_CopyWords(unsigned int *src, unsigned int *dst, unsigned int size);

void Seq_RestorePrimaryState(void) {
    u32 i;
    AkaoTrack *track;
    u32 flags;
    u16 value;

    if (*(u32 *)(g_AkaoCurTrack + 4) != 0) {
        Util_CopyWords(g_AkaoCurTrack, g_AkaoTrackStateBackup, 0x68);
        Util_CopyWords((u32 *)&g_AkaoVoiceBanks[0], (u32 *)&g_AkaoVoiceStateBackup[0], sizeof(AkaoVoiceBank));

        flags = *(u32 *)g_AkaoTrackStateBackup;
        if ((flags & 0x100) != 0) {
            i = 0;
            track = g_AkaoVoiceStateBackup[0].tracks;
            do {
                value = track->note_pitch;
                if (value >= 0x50) {
                    track->note_pitch = value - 0x30;
                }
                i++;
                track++;
            } while (i < 24);
        }
    }
}
