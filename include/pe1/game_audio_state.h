#ifndef PE1_GAME_AUDIO_STATE_H
#define PE1_GAME_AUDIO_STATE_H

#include "common.h"

/* Game-side sound command state shared with field effects. */
typedef struct AkaoRuntimeState {
    u8 reset_pending;
    u8 deferred_count;
    s8 defer_enabled;
    u8 pad003[0xCD];
    s8 selected_id;
    u8 selected_value;
    u8 pad0D2[4];
    s8 cd_volume;
    u8 pad0D7[0xF];
    s8 position_x;
    s8 position_z;
    s16 position_y;
    s16 position_w;
    u8 pad0EC[0x28];
    u8 *selected_data;
    u8 *voice_banks[25];
    u8 *archive;
} AkaoRuntimeState;

PE1_STATIC_ASSERT(PE1_OFFSETOF(AkaoRuntimeState, voice_banks) == 0x118, game_audio_voice_bank_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(AkaoRuntimeState, archive) == 0x17C, game_audio_archive_offset);

extern AkaoRuntimeState D_800B0CE8;

#endif
