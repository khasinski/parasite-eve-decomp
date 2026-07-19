
typedef unsigned char AkaoU8;
typedef unsigned short AkaoU16;
typedef unsigned int AkaoU32;
typedef signed short AkaoS16;
enum AkaoVoiceParamFlags
{
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
  AKAO_VOICE_PARAM_ADSR_ATTACK = AKAO_VOICE_PARAM_ADSR_ATTACK_MODE | AKAO_VOICE_PARAM_ADSR_ATTACK_RATE,
  AKAO_VOICE_PARAM_ADSR_SUSTAIN = AKAO_VOICE_PARAM_ADSR_SUSTAIN_MODE | AKAO_VOICE_PARAM_ADSR_SUSTAIN_RATE,
  AKAO_VOICE_PARAM_ADSR_RELEASE = AKAO_VOICE_PARAM_ADSR_RELEASE_MODE | AKAO_VOICE_PARAM_ADSR_RELEASE_RATE,
  AKAO_VOICE_PARAM_ADSR_ALL = (((AKAO_VOICE_PARAM_ADSR_ATTACK | AKAO_VOICE_PARAM_ADSR_DECAY_RATE) | AKAO_VOICE_PARAM_ADSR_SUSTAIN) | AKAO_VOICE_PARAM_ADSR_RELEASE) | AKAO_VOICE_PARAM_ADSR_SUSTAIN_LEVEL,
  AKAO_VOICE_PARAM_ADSR_DECAY_SUSTAIN = AKAO_VOICE_PARAM_ADSR_DECAY_RATE | AKAO_VOICE_PARAM_ADSR_SUSTAIN_LEVEL,
  AKAO_VOICE_PARAM_INSTRUMENT_NO_RELEASE = ((((AKAO_VOICE_PARAM_START_ADDR | AKAO_VOICE_PARAM_LOOP_ADDR) | AKAO_VOICE_PARAM_ADSR_ATTACK) | AKAO_VOICE_PARAM_ADSR_DECAY_RATE) | AKAO_VOICE_PARAM_ADSR_SUSTAIN) | AKAO_VOICE_PARAM_ADSR_SUSTAIN_LEVEL,
  AKAO_VOICE_PARAM_INSTRUMENT_FULL = AKAO_VOICE_PARAM_INSTRUMENT_NO_RELEASE | AKAO_VOICE_PARAM_ADSR_RELEASE,
  AKAO_VOICE_PARAM_RESUME = ((AKAO_VOICE_PARAM_VOLUME | AKAO_VOICE_PARAM_PITCH) | AKAO_VOICE_PARAM_ADSR_ATTACK) | AKAO_VOICE_PARAM_ADSR_SUSTAIN
};
enum AkaoTrackFlags
{
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
  AKAO_TRACK_FLAG_STOP_CLEAR_MASK = (((AKAO_TRACK_FLAG_PITCH_LFO | AKAO_TRACK_FLAG_VOLUME_LFO) | AKAO_TRACK_FLAG_AUX_LFO) | AKAO_TRACK_FLAG_10) | AKAO_TRACK_FLAG_20
};
enum AkaoGlobalUpdateFlags
{
  AKAO_GLOBAL_UPDATE_TEMPO = 1 << 7,
  AKAO_GLOBAL_UPDATE_VOICE_MODES = 1 << 8
};
enum AkaoVoiceMask
{
  AKAO_SPU_VOICE_SFX_START_INDEX = 12,
  AKAO_SPU_VOICE_SFX_START_MASK = 1 << AKAO_SPU_VOICE_SFX_START_INDEX
};
typedef struct AkaoInstrument
{
  AkaoU32 start_address;
  AkaoU32 loop_address;
  AkaoU8 adsr_attack_rate;
  AkaoU8 adsr_decay_rate;
  AkaoU8 adsr_sustain_level;
  AkaoU8 adsr_sustain_rate;
  AkaoU8 adsr_release_rate;
  AkaoU8 adsr_attack_mode;
  AkaoU8 adsr_sustain_mode;
  AkaoU8 adsr_release_mode;
  AkaoU32 pitch[12];
} AkaoInstrument;
typedef struct AkaoVoiceLoopRange
{
  AkaoU8 note;
  AkaoU8 min_value;
  AkaoU8 max_value;
  AkaoU8 adsr_attack_rate;
  AkaoU8 adsr_sustain_rate;
  AkaoU8 adsr_sustain_mode;
  AkaoU8 adsr_release_rate;
  AkaoU8 field_07;
} AkaoVoiceLoopRange;
typedef struct AkaoVoiceParams
{
  int assigned_voice_index;
  AkaoU32 flags;
  AkaoU32 start_address;
  AkaoU32 loop_address;
  int adsr_attack_mode;
  int adsr_sustain_mode;
  int adsr_release_mode;
  AkaoU16 pitch;
  AkaoU16 adsr_attack_rate;
  AkaoU16 adsr_decay_rate;
  AkaoU16 adsr_sustain_level;
  AkaoU16 adsr_sustain_rate;
  AkaoU16 adsr_release_rate;
  AkaoS16 volume_left;
  AkaoS16 volume_right;
} AkaoVoiceParams;
typedef struct AkaoTrack
{
  AkaoU8 *pc;
  AkaoU8 *call_stack[4];
  AkaoU8 *branch_target;
  AkaoU8 *repeat_target;
  void *pitch_lfo_table;
  void *volume_lfo_table;
  void *aux_lfo_table;
  AkaoU32 key_off_mask;
  AkaoU32 key_on_mask;
  AkaoU32 pitch_base;
  AkaoU32 voice_mask_a;
  AkaoU32 flags;
  AkaoU32 voice_mask_b;
  AkaoU32 pan_base;
  int expression_value;
  int expression_delta;
  int field_4C;
  AkaoU16 field_50_duration;
  AkaoU16 field_52_duration;
  AkaoU16 parent_track_id;
  AkaoU16 field_56;
  AkaoU16 pan_duration;
  AkaoU16 note_pitch;
  AkaoU16 voice_index;
  AkaoU16 pitch_slide_current;
  AkaoU16 pitch_slide_duration;
  AkaoU16 repeat_counters[4];
  AkaoS16 volume;
  AkaoU16 volume_base;
  AkaoU16 volume_duration;
  AkaoU16 field_70;
  AkaoU16 expression_duration;
  AkaoU16 panpot_duration;
  AkaoU16 panpot;
  AkaoU16 panpot_slide_duration;
  AkaoU16 field_7A;
  AkaoU16 panpot_step;
  AkaoU16 vibrato_duration;
  AkaoU16 tremolo_counter;
  AkaoU16 tremolo_duration;
  AkaoU16 tremolo_phase;
  unsigned char pad_086[0x02];
  AkaoU16 pitch_lfo_delay;
  AkaoU16 pitch_lfo_counter;
  AkaoU16 pitch_lfo_duration;
  AkaoU16 pitch_lfo_phase;
  AkaoU16 pitch_lfo_selector;
  AkaoU16 pitch_lfo_depth;
  AkaoU16 pitch_lfo_target;
  AkaoU16 pitch_lfo_slide_duration;
  AkaoS16 pitch_lfo_delta;
  AkaoS16 field_9A;
  AkaoU16 volume_lfo_delay;
  AkaoU16 volume_lfo_counter;
  AkaoU16 volume_lfo_duration;
  AkaoU16 volume_lfo_phase;
  AkaoU16 volume_lfo_selector;
  AkaoU16 volume_lfo_target;
  AkaoU16 volume_lfo_slide_duration;
  AkaoS16 volume_lfo_delta;
  AkaoS16 field_AC;
  AkaoU16 aux_lfo_duration;
  AkaoU16 aux_lfo_phase;
  AkaoU16 aux_lfo_selector;
  AkaoU16 aux_lfo_target;
  AkaoU16 aux_lfo_slide_duration;
  AkaoS16 aux_lfo_delta;
  AkaoU16 key_on_delay;
  AkaoU16 key_off_delay;
  unsigned char pad_0BE[0x10];
  AkaoU16 call_stack_index;
  AkaoS16 field_D0;
  AkaoS16 field_D2;
  AkaoS16 volume_delta;
  AkaoS16 pitch_slide_delta;
  AkaoS16 pan_target;
  AkaoS16 pan_delta;
  AkaoS16 panpot_delta;
  AkaoS16 expression;
  AkaoS16 detune;
  AkaoS16 field_E2;
  AkaoS16 vibrato_delta;
  AkaoS16 tremolo_delta;
  AkaoS16 pitch_lfo_value;
  AkaoS16 volume_lfo_value;
  AkaoS16 aux_lfo_value;
  unsigned char pad_0EE[0x02];
  int assigned_voice_index;
  AkaoU32 update_flags;
  AkaoU32 sample_header;
  AkaoU32 sample_data;
  int adsr_attack;
  int adsr_sustain;
  int adsr_release;
  AkaoU16 pitch;
  AkaoU16 adsr_attack_rate;
  AkaoU16 adsr_decay_rate;
  AkaoU16 adsr_sustain_level;
  AkaoU16 adsr_sustain_rate;
  AkaoU16 adsr_release_rate;
  AkaoS16 volume_left;
  AkaoS16 volume_right;
} AkaoTrack;
typedef void (*AkaoTrackHandler)(AkaoTrack *track);
typedef struct SquareMessageEntry
{
  unsigned char opcode;
  unsigned char status;
  short slot_idx;
  int arg_04;
  int arg_08;
  union 
  {
    struct 
    {
      unsigned short arg_0C;
      unsigned short arg_0E;
    } halves;
    int word;
  } u;
  int unk_10;
  int arg_14;
  int unk_18;
  int arg_1C;
  int arg_20;
} SquareMessageEntry;
typedef SquareMessageEntry BattleCmdEntry;
typedef SquareMessageEntry AkaoQueueEntry;
typedef void (*AkaoMessageHandler)(AkaoQueueEntry *entry);
extern AkaoQueueEntry D_800B8628[];
extern int g_AkaoMessageQueueCount;
extern AkaoMessageHandler Akao_MessageHandlers[] asm("D_8009C0C0");
void Akao_MessageNoop(AkaoQueueEntry *entry);
void Akao_AllocMessageSlot(AkaoQueueEntry **out_msg);
void Akao_ProcessMessageQueue(void);
typedef signed char AkaoCommandS8;
enum AkaoSequenceOpcode
{
  AKAO_SEQ_OP_FINISH_CHANNEL = 0xA0,
  AKAO_SEQ_OP_LOAD_INSTRUMENT = 0xA1,
  AKAO_SEQ_OP_SET_NEXT_NOTE_LENGTH = 0xA2,
  AKAO_SEQ_OP_CHANNEL_MASTER_VOLUME = 0xA3,
  AKAO_SEQ_OP_CHANNEL_VOLUME = 0xA8,
  AKAO_SEQ_OP_CHANNEL_VOLUME_SLIDE = 0xA9,
  AKAO_SEQ_OP_CHANNEL_PAN = 0xAA,
  AKAO_SEQ_OP_CHANNEL_PAN_SLIDE = 0xAB,
  AKAO_SEQ_OP_NOISE_CLOCK = 0xAC,
  AKAO_SEQ_OP_ADSR_ATTACK_RATE = 0xAD,
  AKAO_SEQ_OP_PITCH_BEND_SLIDE = 0xA4,
  AKAO_SEQ_OP_SET_OCTAVE = 0xA5,
  AKAO_SEQ_OP_TRANSPOSE_ABSOLUTE = 0xC0,
  AKAO_SEQ_OP_TRANSPOSE_RELATIVE = 0xC1,
  AKAO_SEQ_OP_REVERB_ON = 0xC2,
  AKAO_SEQ_OP_LOOP_POINT = 0xC8,
  AKAO_SEQ_OP_RETURN_LOOP_N_TIMES = 0xC9,
  AKAO_SEQ_OP_RETURN_LOOP = 0xCA,
  AKAO_SEQ_OP_FULL_LENGTH_NOTE_ON = 0xD0,
  AKAO_SEQ_OP_FULL_LENGTH_NOTE_OFF = 0xD1,
  AKAO_SEQ_OP_FM_TOGGLE_DELAY_ON = 0xD2,
  AKAO_SEQ_OP_PLAYBACK_RATE_SIDE_CHAIN_ON = 0xD4,
  AKAO_SEQ_OP_PLAYBACK_RATE_SIDE_CHAIN_OFF = 0xD5,
  AKAO_SEQ_OP_PITCH_VOLUME_SIDE_CHAIN_ON = 0xD6,
  AKAO_SEQ_OP_FINE_TUNE_ABSOLUTE = 0xD8,
  AKAO_SEQ_OP_FINE_TUNE_RELATIVE = 0xD9,
  AKAO_SEQ_OP_PORTAMENTO_ON = 0xDA,
  AKAO_SEQ_OP_JUMP_ON_REPEAT = 0xF0,
  AKAO_SEQ_OP_BREAK_LOOP_ON_REPEAT = 0xF1
};
typedef struct AkaoValueCommand
{
  int pad_0;
  int field_4;
} AkaoValueCommand;
typedef struct AkaoGlobalSlideCommand
{
  int pad_0;
  int duration;
  AkaoCommandS8 target;
} AkaoGlobalSlideCommand;
typedef struct AkaoGlobalSlideRangeCommand
{
  int pad_0;
  int duration;
  int start;
  AkaoCommandS8 target;
} AkaoGlobalSlideRangeCommand;
typedef struct AkaoGlobalParamCommand
{
  int pad_0;
  AkaoCommandS8 value;
} AkaoGlobalParamCommand;
extern int g_AkaoCmdOpcode;
extern int g_AkaoCmdArg0;
extern int g_AkaoCmdArg1;
extern int g_AkaoCmdArg2;
extern int g_AkaoCmdArg3;
int Akao_EnqueueStagedCommand(void);
void AkaoSpuVoice_SetVolume(unsigned int index, unsigned int left, unsigned int right);
void AkaoSpuVoice_SetPitch(unsigned int index, unsigned int value);
void AkaoSpuVoice_SetStartAddress(unsigned int index, unsigned int value);
void AkaoSpuVoice_SetRepeatAddress(unsigned int index, unsigned int value);
void AkaoSpuVoice_SetAdsrAttack(unsigned int index, unsigned int left, unsigned int right);
void AkaoSpuVoice_SetAdsrDecayRate(unsigned int index, unsigned int value);
void AkaoSpuVoice_SetAdsrSustainLevel(unsigned int index, unsigned int value);
void AkaoSpuVoice_SetAdsrSustainRate(unsigned int index, unsigned int left, unsigned int right);
void AkaoSpuVoice_SetAdsrReleaseRate(unsigned int index, unsigned int left, unsigned int right);
void SeqOp_SetVolumeOrExpression(AkaoTrack *track)
{
  AkaoU32 new_var;
  register unsigned char *cursor;
  int value;
  if ((track->flags & 8) != 0)
  {
    cursor = track->pc;
    track->pc = cursor + 1;
    if (1)
    {
      value = *cursor;
      track->volume = value << 7;
    }
  }
  else
  {
    register unsigned char *cursor2;
    register unsigned char *next;
    register unsigned int flags;
    register int value2;
    cursor2 = track->pc;
    next = cursor2 + 1;
    track->pc = next;
    new_var = track->update_flags;
    value2 = *((signed char *) cursor2);
    flags = new_var;
    track->expression_duration = 0;
    flags |= AKAO_VOICE_PARAM_VOLUME;
    value2 <<= 23;
    track->update_flags = flags;
    track->expression_value = value2;
  }
}
