#ifndef PE1_RENDER_ANIMATION_FRAME_H
#define PE1_RENDER_ANIMATION_FRAME_H

#include "common.h"

typedef struct RenderAnimVramCounter {
    int value;
    char reserved[8];
} RenderAnimVramCounter;

typedef struct RenderAnimFrameData {
    u8 reserved00[2];
    s16 frameCount;
} RenderAnimFrameData;

typedef struct RenderAnimActor {
    u8 reserved00[0x4C];
    int modeFlags;
    u8 reserved50[0x18];
    RenderAnimFrameData *frameData;
} RenderAnimActor;

typedef struct RenderAnimPlayer {
    u8 reserved00[0x28];
    u8 decoderData[2];
    s16 x;
    u8 reserved2C[2];
    s16 y;
    u8 reserved30[2];
    s16 z;
} RenderAnimPlayer;

PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderAnimActor, frameData) == 0x68,
                  render_anim_actor_frame_data_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderAnimFrameData, frameCount) == 2,
                  render_anim_frame_count_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderAnimPlayer, z) == 0x32,
                  render_anim_player_z_offset);

extern int g_BattleAttackAnimFrame;
extern RenderAnimActor *g_ActiveActor[3];
extern RenderAnimPlayer *g_PlayerEntity[3];
extern int g_ActiveDrawSlot[3];
extern RenderAnimVramCounter g_RenderAnimVramOffset;
/* Distinct views reproduce the original load and later update. */
extern RenderAnimVramCounter g_RenderAnimVramOffsetLoad asm("g_RenderAnimVramOffset");
extern RenderAnimVramCounter g_RenderAnimVramOffsetUpdate asm("g_RenderAnimVramOffset");
extern void *D_800B0DF8[3];
extern u8 *g_OtBufferTable[];
extern u8 *g_RenderAnimBufferBase[];

void Asset_Find08Alt(int id, int zero, int x, int y, int z);
void Render_SetGteScreenOffset(void);
int Render_DecompressAnimFrame(void *anim, void *entity, int size, int width,
                               void *ot, void *buffer);
void Render_ResetGteScreenOffset(void);
void Render_AnimationFrame(void);

#endif
