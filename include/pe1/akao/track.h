#ifndef PE1_AKAO_TRACK_H
#define PE1_AKAO_TRACK_H

typedef unsigned char AkaoU8;
typedef unsigned short AkaoU16;
typedef unsigned int AkaoU32;
typedef signed short AkaoS16;

enum AkaoVoiceParamFlags {
    AKAO_VOICE_PARAM_VOLUME_L = 1 << 0,
    AKAO_VOICE_PARAM_VOLUME_R = 1 << 1,
    AKAO_VOICE_PARAM_VOLUME = AKAO_VOICE_PARAM_VOLUME_L | AKAO_VOICE_PARAM_VOLUME_R,
    AKAO_VOICE_PARAM_PITCH = 1 << 4,
    AKAO_VOICE_PARAM_START_ADDR = 1 << 7,
    AKAO_VOICE_PARAM_ADSR_ATTACK_MODE = 1 << 8,
    AKAO_VOICE_PARAM_ADSR_SUSTAIN_MODE = 1 << 9,
    AKAO_VOICE_PARAM_ADSR_RELEASE_MODE = 1 << 10,
    AKAO_VOICE_PARAM_ADSR_ATTACK_RATE = 1 << 11,
    AKAO_VOICE_PARAM_ADSR_DECAY_RATE = 1 << 12,
    AKAO_VOICE_PARAM_ADSR_SUSTAIN_RATE = 1 << 13,
    AKAO_VOICE_PARAM_ADSR_RELEASE_RATE = 1 << 14,
    AKAO_VOICE_PARAM_ADSR_SUSTAIN_LEVEL = 1 << 15,
    AKAO_VOICE_PARAM_LOOP_ADDR = 1 << 16,
    AKAO_VOICE_PARAM_ADSR_ATTACK =
        AKAO_VOICE_PARAM_ADSR_ATTACK_MODE | AKAO_VOICE_PARAM_ADSR_ATTACK_RATE,
    AKAO_VOICE_PARAM_ADSR_SUSTAIN =
        AKAO_VOICE_PARAM_ADSR_SUSTAIN_MODE | AKAO_VOICE_PARAM_ADSR_SUSTAIN_RATE,
    AKAO_VOICE_PARAM_ADSR_RELEASE =
        AKAO_VOICE_PARAM_ADSR_RELEASE_MODE | AKAO_VOICE_PARAM_ADSR_RELEASE_RATE,
    AKAO_VOICE_PARAM_ADSR_ALL =
        AKAO_VOICE_PARAM_ADSR_ATTACK | AKAO_VOICE_PARAM_ADSR_DECAY_RATE |
        AKAO_VOICE_PARAM_ADSR_SUSTAIN | AKAO_VOICE_PARAM_ADSR_RELEASE |
        AKAO_VOICE_PARAM_ADSR_SUSTAIN_LEVEL,
    AKAO_VOICE_PARAM_ADSR_DECAY_SUSTAIN =
        AKAO_VOICE_PARAM_ADSR_DECAY_RATE | AKAO_VOICE_PARAM_ADSR_SUSTAIN_LEVEL,
    AKAO_VOICE_PARAM_INSTRUMENT_NO_RELEASE =
        AKAO_VOICE_PARAM_START_ADDR | AKAO_VOICE_PARAM_LOOP_ADDR |
        AKAO_VOICE_PARAM_ADSR_ATTACK | AKAO_VOICE_PARAM_ADSR_DECAY_RATE |
        AKAO_VOICE_PARAM_ADSR_SUSTAIN | AKAO_VOICE_PARAM_ADSR_SUSTAIN_LEVEL,
    AKAO_VOICE_PARAM_INSTRUMENT_FULL =
        AKAO_VOICE_PARAM_INSTRUMENT_NO_RELEASE | AKAO_VOICE_PARAM_ADSR_RELEASE,
    AKAO_VOICE_PARAM_RESUME =
        AKAO_VOICE_PARAM_VOLUME | AKAO_VOICE_PARAM_PITCH |
        AKAO_VOICE_PARAM_ADSR_ATTACK | AKAO_VOICE_PARAM_ADSR_SUSTAIN
};

enum AkaoTrackFlags {
    AKAO_TRACK_FLAG_PITCH_LFO = 1 << 0,
    AKAO_TRACK_FLAG_VOLUME_LFO = 1 << 1,
    AKAO_TRACK_FLAG_AUX_LFO = 1 << 2,
    AKAO_TRACK_FLAG_BRANCH_ACTIVE = 1 << 3,
    AKAO_TRACK_FLAG_10 = 1 << 4,
    AKAO_TRACK_FLAG_20 = 1 << 5,
    AKAO_TRACK_FLAG_RELEASE_RATE_OVERRIDE = 1 << 9,
    AKAO_TRACK_FLAG_VOICE_ALLOCATED = 1 << 11,
    AKAO_TRACK_FLAG_PENDING_NOTE_PITCH = 1 << 12,
    AKAO_TRACK_FLAG_SUSTAIN = 1 << 20,
    AKAO_TRACK_FLAG_STOP_CLEAR_MASK =
        AKAO_TRACK_FLAG_PITCH_LFO | AKAO_TRACK_FLAG_VOLUME_LFO |
        AKAO_TRACK_FLAG_AUX_LFO | AKAO_TRACK_FLAG_10 | AKAO_TRACK_FLAG_20
};

enum AkaoGlobalUpdateFlags {
    AKAO_GLOBAL_UPDATE_TEMPO = 1 << 7,
    AKAO_GLOBAL_UPDATE_VOICE_MODES = 1 << 8
};

enum AkaoVoiceMask {
    AKAO_SPU_VOICE_SFX_START_INDEX = 12,
    AKAO_SPU_VOICE_SFX_START_MASK = 1 << AKAO_SPU_VOICE_SFX_START_INDEX
};

typedef struct AkaoInstrument {
    /* 0x00 */ AkaoU32 start_address;
    /* 0x04 */ AkaoU32 loop_address;
    /* 0x08 */ AkaoU8 adsr_attack_rate;
    /* 0x09 */ AkaoU8 adsr_decay_rate;
    /* 0x0A */ AkaoU8 adsr_sustain_level;
    /* 0x0B */ AkaoU8 adsr_sustain_rate;
    /* 0x0C */ AkaoU8 adsr_release_rate;
    /* 0x0D */ AkaoU8 adsr_attack_mode;
    /* 0x0E */ AkaoU8 adsr_sustain_mode;
    /* 0x0F */ AkaoU8 adsr_release_mode;
    /* 0x10 */ AkaoU32 pitch[12];
} AkaoInstrument;

typedef struct AkaoVoiceLoopRange {
    /* 0x00 */ AkaoU8 note;
    /* 0x01 */ AkaoU8 min_value;
    /* 0x02 */ AkaoU8 max_value;
    /* 0x03 */ AkaoU8 adsr_attack_rate;
    /* 0x04 */ AkaoU8 adsr_sustain_rate;
    /* 0x05 */ AkaoU8 adsr_sustain_mode;
    /* 0x06 */ AkaoU8 adsr_release_rate;
    /* 0x07 */ AkaoU8 field_07;
} AkaoVoiceLoopRange;

typedef struct AkaoVoiceParams {
    /* 0x00 */ int assigned_voice_index;
    /* 0x04 */ AkaoU32 flags;
    /* 0x08 */ AkaoU32 start_address;
    /* 0x0C */ AkaoU32 loop_address;
    /* 0x10 */ int adsr_attack_mode;
    /* 0x14 */ int adsr_sustain_mode;
    /* 0x18 */ int adsr_release_mode;
    /* 0x1C */ AkaoU16 pitch;
    /* 0x1E */ AkaoU16 adsr_attack_rate;
    /* 0x20 */ AkaoU16 adsr_decay_rate;
    /* 0x22 */ AkaoU16 adsr_sustain_level;
    /* 0x24 */ AkaoU16 adsr_sustain_rate;
    /* 0x26 */ AkaoU16 adsr_release_rate;
    /* 0x28 */ AkaoS16 volume_left;
    /* 0x2A */ AkaoS16 volume_right;
} AkaoVoiceParams;

typedef struct AkaoTrack {
    /* 0x000 */ AkaoU8 *pc;
    /* 0x004 */ AkaoU8 *call_stack[4];
    /* 0x014 */ AkaoU8 *branch_target;
    /* 0x018 */ AkaoU8 *repeat_target;
    /* 0x01C */ void *pitch_lfo_table;
    /* 0x020 */ void *volume_lfo_table;
    /* 0x024 */ void *aux_lfo_table;
    /* 0x028 */ AkaoU32 key_off_mask;
    /* 0x02C */ AkaoU32 key_on_mask;
    /* 0x030 */ AkaoU32 pitch_base;
    /* 0x034 */ AkaoU32 voice_mask_a;
    /* 0x038 */ AkaoU32 flags;
    /* 0x03C */ AkaoU32 voice_mask_b;
    /* 0x040 */ AkaoU32 pan_base;
    /* 0x044 */ int expression_value;
    /* 0x048 */ int expression_delta;
    /* 0x04C */ int field_4C;
    /* 0x050 */ AkaoU16 field_50_duration;
    /* 0x052 */ AkaoU16 field_52_duration;
    /* 0x054 */ AkaoU16 parent_track_id;
    /* 0x056 */ AkaoU16 field_56;
    /* 0x058 */ AkaoU16 pan_duration;
    /* 0x05A */ AkaoU16 note_pitch;
    /* 0x05C */ AkaoU16 voice_index;
    /* 0x05E */ AkaoU16 pitch_slide_current;
    /* 0x060 */ AkaoU16 pitch_slide_duration;
    /* 0x062 */ AkaoU16 repeat_counters[4];
    /* 0x06A */ AkaoS16 volume;
    /* 0x06C */ AkaoU16 volume_base;
    /* 0x06E */ AkaoU16 volume_duration;
    /* 0x070 */ AkaoU16 field_70;
    /* 0x072 */ AkaoU16 expression_duration;
    /* 0x074 */ AkaoU16 panpot_duration;
    /* 0x076 */ AkaoU16 panpot;
    /* 0x078 */ AkaoU16 panpot_slide_duration;
    /* 0x07A */ AkaoU16 field_7A;
    /* 0x07C */ AkaoU16 panpot_step;
    /* 0x07E */ AkaoU16 vibrato_duration;
    /* 0x080 */ AkaoU16 tremolo_counter;
    /* 0x082 */ AkaoU16 tremolo_duration;
    /* 0x084 */ AkaoU16 tremolo_phase;
    /* 0x086 */ unsigned char pad_086[0x02];
    /* 0x088 */ AkaoU16 pitch_lfo_delay;
    /* 0x08A */ AkaoU16 pitch_lfo_counter;
    /* 0x08C */ AkaoU16 pitch_lfo_duration;
    /* 0x08E */ AkaoU16 pitch_lfo_phase;
    /* 0x090 */ AkaoU16 pitch_lfo_selector;
    /* 0x092 */ AkaoU16 pitch_lfo_depth;
    /* 0x094 */ AkaoU16 pitch_lfo_target;
    /* 0x096 */ AkaoU16 pitch_lfo_slide_duration;
    /* 0x098 */ AkaoS16 pitch_lfo_delta;
    /* 0x09A */ AkaoS16 field_9A;
    /* 0x09C */ AkaoU16 volume_lfo_delay;
    /* 0x09E */ AkaoU16 volume_lfo_counter;
    /* 0x0A0 */ AkaoU16 volume_lfo_duration;
    /* 0x0A2 */ AkaoU16 volume_lfo_phase;
    /* 0x0A4 */ AkaoU16 volume_lfo_selector;
    /* 0x0A6 */ AkaoU16 volume_lfo_target;
    /* 0x0A8 */ AkaoU16 volume_lfo_slide_duration;
    /* 0x0AA */ AkaoS16 volume_lfo_delta;
    /* 0x0AC */ AkaoS16 field_AC;
    /* 0x0AE */ AkaoU16 aux_lfo_duration;
    /* 0x0B0 */ AkaoU16 aux_lfo_phase;
    /* 0x0B2 */ AkaoU16 aux_lfo_selector;
    /* 0x0B4 */ AkaoU16 aux_lfo_target;
    /* 0x0B6 */ AkaoU16 aux_lfo_slide_duration;
    /* 0x0B8 */ AkaoS16 aux_lfo_delta;
    /* 0x0BA */ AkaoU16 key_on_delay;
    /* 0x0BC */ AkaoU16 key_off_delay;
    /* 0x0BE */ unsigned char pad_0BE[0x10];
    /* 0x0CE */ AkaoU16 call_stack_index;
    /* 0x0D0 */ AkaoS16 field_D0;
    /* 0x0D2 */ AkaoS16 field_D2;
    /* 0x0D4 */ AkaoS16 volume_delta;
    /* 0x0D6 */ AkaoS16 pitch_slide_delta;
    /* 0x0D8 */ AkaoS16 pan_target;
    /* 0x0DA */ AkaoS16 pan_delta;
    /* 0x0DC */ AkaoS16 panpot_delta;
    /* 0x0DE */ AkaoS16 expression;
    /* 0x0E0 */ AkaoS16 detune;
    /* 0x0E2 */ AkaoS16 field_E2;
    /* 0x0E4 */ AkaoS16 vibrato_delta;
    /* 0x0E6 */ AkaoS16 tremolo_delta;
    /* 0x0E8 */ AkaoS16 pitch_lfo_value;
    /* 0x0EA */ AkaoS16 volume_lfo_value;
    /* 0x0EC */ AkaoS16 aux_lfo_value;
    /* 0x0EE */ unsigned char pad_0EE[0x02];
    /* 0x0F0 */ int assigned_voice_index;
    /* 0x0F4 */ AkaoU32 update_flags;
    /* 0x0F8 */ AkaoU32 sample_header;
    /* 0x0FC */ AkaoU32 sample_data;
    /* 0x100 */ int adsr_attack;
    /* 0x104 */ int adsr_sustain;
    /* 0x108 */ int adsr_release;
    /* 0x10C */ AkaoU16 pitch;
    /* 0x10E */ AkaoU16 adsr_attack_rate;
    /* 0x110 */ AkaoU16 adsr_decay_rate;
    /* 0x112 */ AkaoU16 adsr_sustain_level;
    /* 0x114 */ AkaoU16 adsr_sustain_rate;
    /* 0x116 */ AkaoU16 adsr_release_rate;
    /* 0x118 */ AkaoS16 volume_left;
    /* 0x11A */ AkaoS16 volume_right;
} AkaoTrack;

#define AKAO_TRACK_VOICE(track) (*(AkaoVoiceParams *)&(track)->assigned_voice_index)

typedef void (*AkaoTrackHandler)(AkaoTrack *track);

#endif
