/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
#include "pe1/scene_assets.h"
#include "pe1/psyq_nop.h"
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

extern u32 D_800B0CD8[];
extern void *D_800B0E08[];
extern void *tableCheck[] asm("D_800B0E08");
extern void *tableCheckAgain[] asm("D_800B0E08");
extern void *tableAgain[] asm("D_800B0E08");
extern int D_8009CDA4[];
extern int D_8009D188, D_8009D18C, D_8009D190;
void Akao_Cmd_F1(void);
void Akao_Cmd_40(void);
int Akao_Cmd_10(u32);
void Akao_SendTableCommand(void *, int, int, int, int);
void Akao_Cmd_C1_WithSlot(int, int, int);
void Akao_Cmd_C0_WithSlot(int, int);
void Overlay_RegisterAudioSlot(int, int, int, int);

/* Matching debt: one argument pin, one empty flag barrier, two individually
 * wrapped NOPs, three table aliases and the retry jump. The aliases preserve
 * independent absolute reads of the table pointer around callbacks. */
int CD_StepReadState(int active)
{
    Pe1GameState *state = &g_GameState;
    int slot;
    int voice;
    u32 flags;
    u32 changing;
retry:
    {
        switch (state->cd_transition_phase) {
        case 0:
            D_8009D188 = 0;
            Akao_Cmd_F1();
            if (active) {
                if (!(D_800B0CD8[0] & 0x400000)) {
                    if (*tableCheck) {
                        Akao_SendTableCommand(*D_800B0E08, 0x451, 0, 0x80, 0x7F);
                        if (*tableCheckAgain)
                            Akao_SendTableCommand(*tableAgain, 0x452, 0, 0x80, 0x7F);
                    }
                }
                state->cd_transition_phase = 44;
            } else {
                if (state->flags & 4) {
                    register int zero asm("$4") = 0;
                    int duration = 60;
                    int now = *D_8009CDA4;
                    PE1_NOP_IO2_DEP(zero, duration, now);
                    D_8009D18C = now;
                    Akao_Cmd_C1_WithSlot(zero, duration, 0);
                }
                state->cd_transition_phase = 45;
            }
            break;
        case 44:
            if (state->scene_audio.tracks.pending_key != state->scene_audio.tracks.keys[0][0]) {
                if (state->flags & 0x40) {
                    int zero = 0;
                    int duration = 60;
                    int now = *D_8009CDA4;
                    PE1_NOP_IO2_DEP(zero, duration, now);
                    D_8009D18C = now;
                    Akao_Cmd_C1_WithSlot(zero, duration, 0);
                }
                state->flags |= 4;
            }
            state->cd_transition_phase = 46;
            break;
        case 46:
            if (CD_FindNextDataSector() == 1)
                return 1;
            flags = state->flags;
            changing = flags & 4;
            asm("" : "=r"(changing) : "0"(changing));
            if (changing) {
                if (flags & 0x40) {
                    D_8009D190 = 60 - (*D_8009CDA4 - D_8009D18C);
                    if (D_8009D190 > 8) {
                        D_8009D190 = 8;
                        Akao_Cmd_C1_WithSlot(0, 16, 0);
                    } else if (D_8009D190 < 0) {
                        D_8009D190 = 0;
                    }
                }
            }
            state->cd_transition_phase = 63;
            break;
        case 63:
            if (state->flags & 4) {
                if (state->flags & 0x40) {
                    if (D_8009D190 > 0) {
                        --D_8009D190;
                        return 1;
                    }
                    Akao_Cmd_40();
                    Akao_Cmd_F0();
                }
                state->cd_transition_phase = 47;
            } else {
                state->cd_transition_phase = 48;
            }
            break;
        case 47:
            if (CD_ReadSectors(0, state->scene_audio.tracks.pending_bank, 0,
                               state->scene_load_scratch, 0x21, 0) == 1)
                return 1;
            state->cd_transition_phase = 48;
            break;
        case 48:
            Akao_Cmd_10(state->bank_asset_table);
            Akao_Cmd_C0_WithSlot(0, 0x7F);
            state->cd_transition_phase = 0;
            return 0;
        case 45:
            if (D_8009D188 < 2) {
                if (state->pending_stream_banks[D_8009D188]) {
                    state->cd_transition_phase = 49;
                    break;
                }
                ++D_8009D188;
            }
            state->cd_transition_phase = 50;
            break;
        case 49:
            if (CD_ReadSectors(2, state->pending_stream_banks[D_8009D188], D_8009D188,
                               state->scene_load_scratch, 0x21, 0) == 1)
                return 1;
            ++D_8009D188;
            state->cd_transition_phase = 45;
            break;
        case 50:
            if (state->pending_sample_bank != -1 &&
                CD_ReadSectors(1, state->pending_sample_bank, 0,
                               state->scene_load_scratch, 0x21, 0) == 1)
                return 1;
            if (state->flags & 4) {
                D_8009D190 = 60 - (*D_8009CDA4 - D_8009D18C);
                if (D_8009D190 > 8) {
                    D_8009D190 = 8;
                    Akao_Cmd_C1_WithSlot(0, 16, 0);
                } else if (D_8009D190 < 0) {
                    D_8009D190 = 0;
                }
            }
            state->cd_transition_phase = 64;
            break;
        case 64:
            if (state->flags & 4) {
                if (D_8009D190 > 0) {
                    --D_8009D190;
                    return 1;
                }
                Akao_Cmd_F0();
                if (state->flags & 0x40) {
                    state->flags &= ~0x40;
                    state->cd_transition_phase = 62;
                    break;
                }
            }
            state->cd_transition_phase = 0;
            state->flags &= ~4;
            return 0;
        case 62:
            if (CD_ReadSectors(0, ((s8 *)state->scene_audio.tracks.banks)[0], 0,
                               state->scene_load_scratch, 0x21, 0) == 1)
                return 1;
            state->cd_transition_phase = 51;
            break;
        case 51:
            if (CD_SeekToTrack(state->scene_audio.tracks.keys[0][0], 1, 0, &slot, 0) == 1)
                return 1;
            voice = Akao_Cmd_10(state->bank_work_base);
            if (voice == -1) {
                state->cd_transition_phase = 62;
                return 1;
            }
            Akao_Cmd_C1_WithSlot(voice, 60, state->transition_volume);
            if (voice)
                Overlay_RegisterAudioSlot(0, state->scene_audio.tracks.keys[0][0], voice,
                                          state->transition_volume);
            state->cd_transition_phase = 0;
            state->flags = (state->flags & ~4) | 0x40;
            return 0;
        default:
            return 0;
        }
        goto retry;
    }
}
