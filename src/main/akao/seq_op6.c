typedef unsigned int u32;

#include "pe1/akao.h"

extern char *g_AkaoCurTrack;
extern u32 g_AkaoTrack3CMask;

void Seq_MarkTrack3CMaskDirty(void);

void SeqOp_SetMask(AkaoTrack *track, u32 mask) {
    if (track->parent_track_id == 0) {
        ((AkaoTrack *)g_AkaoCurTrack)->voice_mask_b |= mask;
    } else if ((track->flags & 0x10000) != 0) {
        g_AkaoTrack3CMask |= mask;
    }

    Seq_MarkTrack3CMaskDirty();
}

void SeqOp_ClearTrack3CMask(AkaoTrack *track, u32 mask) {
    if (track->parent_track_id == 0) {
        ((AkaoTrack *)g_AkaoCurTrack)->voice_mask_b &= ~mask;
    } else {
        g_AkaoTrack3CMask &= ~mask;
    }

    Seq_MarkTrack3CMaskDirty();
    track->key_off_delay = 0;
}
