#include "common.h"
#include "pe1/akao/voice_state.h"

void Akao_UpdateVoiceMask(int new_base) {
    AkaoTrack *track;
    AkaoTrack *active;
    AkaoSequencerBank *bank;
    register unsigned int bit asm("$7");
    register unsigned int remaining asm("$8");
    unsigned int mask;
    unsigned int voice_mask;
    register unsigned int updated asm("$2");
    int delta;
    int backup_base;
    unsigned int old_status, allocation;
    AkaoSequencerBank *loaded_bank;
    unsigned int key_on_state;
    unsigned int valid_bits;
    register unsigned int pending asm("$3");
    unsigned int restore_mask;
    AkaoSequencerBank *read_bank;
    AkaoSequencerBank *clear_bank;
    AkaoSequencerBank *final_bank;
    register AkaoU16 *duration asm("$5");
    unsigned int magic_flags;
    register unsigned int reset_duration asm("$10");

    Util_CopyWords((unsigned int *)&g_AkaoTrackStateBackup,
                   (unsigned int *)g_AkaoCurTrack, 0x68);
    Util_CopyWords((unsigned int *)&g_AkaoVoiceStateBackup,
                   (unsigned int *)&g_AkaoVoiceStateTable, sizeof(AkaoVoiceBank));

    track = g_AkaoVoiceStateTable.tracks;
    remaining = AKAO_VOICE_COUNT;
    bit = 1;
    magic_flags = 0x1ff93;
    reset_duration = 4;
    duration = &track->pan_duration;
    /* Keep the loop constants live before loading the sequencer bank. */
    asm volatile("" : "=r"(remaining), "=r"(bit), "=r"(magic_flags),
                      "=r"(reset_duration), "=r"(duration)
                    : "0"(remaining), "1"(bit), "2"(magic_flags),
                      "3"(reset_duration), "4"(duration));
    loaded_bank = g_AkaoCurTrack;
    key_on_state = g_AkaoVoiceKeyOnState;
    asm("" : "=r"(bank) : "0"(loaded_bank), "r"(key_on_state));
    old_status = bank->status_flags;
    allocation = bank->allocated_voice_mask;
    bank->field_20[3] = new_base;
    key_on_state &= 0x100;
    old_status |= key_on_state;
    bank->status_flags = old_status;
    bank->key_on_request_mask = allocation;
    backup_base = g_AkaoTrackStateBackup.field_20[3];
    asm volatile("" : "=r"(backup_base) : "0"(backup_base));
    delta = new_base - backup_base;
    g_AkaoVoiceUpdateFlags |= 0x90;
    asm volatile("" ::: "memory");
    voice_mask = bank->active_voice_mask;
    do {
        /* The original loop addresses these fields from pan_duration. */
        active = (AkaoTrack *)((char *)duration - 0x58);
        if (voice_mask & bit) {
            track->pc += delta;
            active->branch_target += delta;
            active->call_stack[0] += delta;
            active->call_stack[1] += delta;
            active->call_stack[2] += delta;
            active->call_stack[3] += delta;
            duration[-1] += 2;
            duration[0] += 2;
            active->update_flags |= magic_flags;
            if ((bank->status_flags & 0x100) && duration[1] >= 0x20) {
                duration[1] += 0x30;
            }
        } else {
            register AkaoU8 *default_pc asm("$2");
            register AkaoU16 short_duration asm("$2");
            /* These pins keep the constants inside this branch. */
            duration[-1] = reset_duration;
            short_duration = 2;
            duration[0] = short_duration;
            default_pc = (AkaoU8 *)g_AkaoDefaultVoiceProgram;
            track->pc = default_pc;
        }
        --remaining;
        duration += sizeof(AkaoTrack) / sizeof(AkaoU16);
        track++;
        bit <<= 1;
    } while (remaining != 0);

    read_bank = g_AkaoCurTrack;
    updated = Akao_ForEachVoiceMasked(g_AkaoVoiceStateTable2,
                                      ((AkaoSequencerState *)read_bank)->secondary.active_voice_mask &
                                      ((AkaoSequencerState *)read_bank)->secondary.pending_voice_mask);
    valid_bits = 0xffffff;
    clear_bank = g_AkaoCurTrack;
    updated = ~updated;
    clear_bank->key_off_request_mask = 0;
    asm volatile("" ::: "memory");
    mask = ~g_SpuActiveVoiceMask;
    mask &= valid_bits;
    updated &= mask;
    pending = g_SpuPendingKeyOffMask;
    pending |= updated;
    g_SpuPendingKeyOffMask = pending;
    Seq_MarkTrack34MaskDirty();
    Seq_MarkTrack38MaskDirty();
    Seq_MarkTrack3CMaskDirty();
    g_AkaoSelectedBankId = 0;
    if (g_AkaoSeqPendingFlags & 1) {
        final_bank = g_AkaoCurTrack;
        restore_mask = final_bank->active_voice_mask;
        final_bank->active_voice_mask = 0;
        final_bank->pending_restore_mask = restore_mask;
    }
}
