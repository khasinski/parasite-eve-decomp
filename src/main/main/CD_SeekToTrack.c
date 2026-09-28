/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
#include "pe1/scene_assets.h"
extern SceneAssetBlob *D_800B0E64[];
extern SceneTrackRecord *D_8009D180;
extern s16 D_8009D184;
void Akao_Cmd_F0(void);
void *memcpy(void *, const void *, unsigned);

/* Matching debt: finished is pinned to s1. The empty count barrier preserves
 * the signed loop test and its scheduling; volatile directory reads retain
 * the original pair of loads. The shifted state view retains the workspace
 * selection made before callbacks. Neither ASM construct emits instructions. */
int CD_SeekToTrack(int key, int mode, int group, int *slot, int blocking)
{
    int busy = 0;
    register int finished asm("$17") = 0;
    Pe1GameState *state = &g_GameState;
    int bankIndex = group != 0;
    Pe1GameState *selected = (Pe1GameState *)((u8 *)state + bankIndex * 4);
    volatile SceneAssetDirectory *directory = SceneAsset_ResolveOffset(
        *D_800B0E64, (*D_800B0E64)->directoryOffset);
    do {
        switch (state->cd_track_phase) {
        case 0: {
            SceneTrackRecord *record;
            int count;
            int i;
            D_8009D184 = -1;
            record = SceneAsset_ResolveOffset(state->loaded_scene_assets,
                                             directory->trackEntries & 0x3FFFFF);
            count = directory->trackEntries >> 22;
            D_8009D180 = record;
            asm volatile("" : "=r"(count) : "0"(count));
            for (i = 0; i < count; i++, record++) {
                if (record->key == key) {
                    D_8009D184 = record->bank;
                    D_8009D180 = record;
                    break;
                }
            }
            if (D_8009D184 == -1) {
                for (i = 0; i < 2; i++) {
                    if (state->scene_audio.tracks.keys[i][0] == key) {
                        D_8009D184 = (s8)state->scene_audio.tracks.banks[i];
                        D_8009D180 = 0;
                        break;
                    }
                }
                if (D_8009D184 == -1) {
                    *slot = -1;
                    busy = 0;
                    finished = 1;
                    break;
                }
            }
            if (mode) {
                if (state->scene_audio.tracks.keys[0][0] == key) {
                    if (state->flags & 0x40)
                        *slot = -2;
                    else
                        *slot = 0;
                    busy = 0;
                    finished = 1;
                } else if (state->scene_audio.tracks.keys[1][0] == key) {
                    if (state->flags & 0x80)
                        *slot = -2;
                    else
                        *slot = 1;
                    busy = 0;
                    finished = 1;
                } else {
                    if (!group)
                        Akao_Cmd_F0();
                    state->cd_track_phase = 7;
                }
            } else {
                if (state->scene_audio.tracks.keys[0][0] == key) {
                    state->scene_audio.tracks.keys[0][0] = -1;
                    ((s8 *)state->scene_audio.tracks.banks)[0] = -1;
                    state->flags &= ~0x40;
                    *slot = 0;
                }
                if (state->scene_audio.tracks.keys[1][0] == key) {
                    state->scene_audio.tracks.keys[1][0] = -1;
                    ((s8 *)state->scene_audio.tracks.banks)[1] = -1;
                    state->flags &= ~0x80;
                    *slot = 1;
                }
                busy = 0;
                finished = 1;
            }
            break;
        }
        case 7:
            if (CD_ReadSectors(0, D_8009D184, 0, state->scene_load_scratch, 0x21, blocking) == 1) {
                busy = 1;
                finished = (blocking ^ 1) & 1;
            } else {
                state->cd_track_phase = 9;
            }
            break;
        case 9:
            *slot = bankIndex;
            if (D_8009D180) {
                memcpy((void *)selected->bank_work_base,
                       SceneAsset_ResolveOffset(state->loaded_scene_assets,
                                                D_8009D180->offset & 0xFFFFFF),
                       D_8009D180->size & 0xFFFFFF);
            }
            busy = 0;
            finished = 1;
            state->scene_audio.tracks.keys[*slot][0] = key;
            state->scene_audio.tracks.banks[*slot] = D_8009D184;
            state->cd_track_phase = 0;
            break;
        }
    } while (!finished);
    return busy;
}
