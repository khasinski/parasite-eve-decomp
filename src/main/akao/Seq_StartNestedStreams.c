#include "pe1/akao/voice_masks.h"

void Seq_StartNestedStreams(AkaoNestedSource *source, void *first, void *second)
{
    int remaining;
    unsigned bit;
    AkaoTrack *track;
    int active;
    unsigned mask;

    if (!first && !second) {
        return;
    }
    if (source->key_on_mask) {
        Spu_ManageVoices(0, source->key_on_mask);
    }
    do {
        track = &g_AkaoVoiceChannelTable[11];
        active = g_SpuActiveVoiceMask;
        bit = 0x800000;
        mask = active;
        /* Paired streams need adjacent slots; search from the highest voice. */
        if (first && second) {
            remaining = 11;
            --track;
            bit = 0x400000;
            while (1) {
                if (!(mask & (bit | (bit << 1)))) break;
                --remaining;
                --track;
                bit >>= 1;
                if (!remaining) goto exhausted;
            }
        } else {
            remaining = 12;
            while (1) {
                if (!(mask & bit)) break;
                --remaining;
                --track;
                bit >>= 1;
                if (!remaining) break;
            }
        }
        if (!remaining) {
exhausted:
            Spu_ManageVoices(0, 0x40000000);
            if (active == g_SpuActiveVoiceMask) active = 0x80000000;
        }
    } while (!remaining && active >= 0);
    if (active < 0) {
        return;
    }
    if (first) {
        Seq_StartNestedTrack(track, source, bit, first);
        Seq_ClearTrackVoiceId(g_AkaoVoiceStateTable, track->assigned_voice_index);
    }
    if (second) {
        if (first) {
            ++track;
            bit <<= 1;
        }
        Seq_StartNestedTrack(track, source, bit, second);
        Seq_ClearTrackVoiceId(g_AkaoVoiceStateTable, track->assigned_voice_index);
        if (first) track->flags |= 0x10000;
    }
    g_AkaoVoiceUpdateFlags |= 0x10;
    Seq_MarkTrack34MaskDirty();
    Seq_MarkTrack38MaskDirty();
    Seq_MarkTrack3CMaskDirty();
}
