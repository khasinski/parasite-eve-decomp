#ifndef PE1_AKAO_COMMANDS_H
#define PE1_AKAO_COMMANDS_H

#include "pe1/akao/queue.h"

typedef signed char AkaoCommandS8;

enum AkaoSequenceOpcode {
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

typedef struct AkaoValueCommand {
    int pad_0;
    int field_4;
} AkaoValueCommand;

typedef struct AkaoGlobalSlideCommand {
    int pad_0;
    int duration;
    AkaoCommandS8 target;
} AkaoGlobalSlideCommand;

typedef struct AkaoGlobalSlideRangeCommand {
    int pad_0;
    int duration;
    int start;
    AkaoCommandS8 target;
} AkaoGlobalSlideRangeCommand;

typedef struct AkaoGlobalParamCommand {
    int pad_0;
    AkaoCommandS8 value;
} AkaoGlobalParamCommand;

/*
 * Staging globals for the public AKAO command wrappers at 0x800864xx..0x800870xx.
 * The executor consumes these fields, allocates one or more AkaoQueueEntry
 * records, and returns a status value.
 */
extern int g_AkaoCmdOpcode;
extern int g_AkaoCmdArg0;
extern int g_AkaoCmdArg1;
extern int g_AkaoCmdArg2;
extern int g_AkaoCmdArg3;

int Akao_EnqueueStagedCommand(void);

#endif
