/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#include "pe1/render_animation_frame.h"

void Render_AnimationFrame(void) {
    int size;
    int width;
    register int frame asm("$4");
    RenderAnimActor *early_actor;
    register RenderAnimActor *late_actor asm("$4");
    RenderAnimPlayer *player;
    int current_frame;
    int draw_index;
    int delta;
    int scaled_frame;
    /* Preserves the original GCC 2.7.2 stack frame. */
    char frame_work[12];
    int animation_id;
    int zero_arg;
    int x;
    int y;
    int z;

    frame = g_BattleAttackAnimFrame;
    if ((u32)frame < 10U) {
        early_actor = g_ActiveActor[0];
        if ((early_actor->modeFlags & 0x30) == 0x10) {
            scaled_frame = (frame << 2) + frame;
            size = (u32)scaled_frame << 18;
        } else {
            size = (((u32)early_actor->frameData->frameCount << 16) * frame) / 10U;
        }
        current_frame = g_BattleAttackAnimFrame;
        width = current_frame << 6;
    } else {
        late_actor = g_ActiveActor[0];
        size = 0xC80000;
        if ((late_actor->modeFlags & 0x30) != 0x10) {
            size = (u32)late_actor->frameData->frameCount << 16;
        }
        width = (g_BattleAttackAnimFrame << 4) + 10;
        asm volatile("" : : : "memory");
        current_frame = g_BattleAttackAnimFrame;
    }

    animation_id = 0x457;
    if (current_frame == 0) {
        player = g_PlayerEntity[0];
        x = player->x;
        y = player->y;
        z = player->z;
        zero_arg = 0;
        Asset_Find08Alt(animation_id, zero_arg, x, y, z);
    }

    Render_SetGteScreenOffset();

    draw_index = g_ActiveDrawSlot[0];
    delta = Render_DecompressAnimFrame(D_800B0DF8[0],
                                       g_PlayerEntity[0]->decoderData,
                                       size, width & 0xFFFE,
                                       g_OtBufferTable[draw_index] + 8,
                                       g_RenderAnimBufferBase[draw_index] +
                                           g_RenderAnimVramOffsetLoad.value);
    g_RenderAnimVramOffset.value = g_RenderAnimVramOffsetUpdate.value + delta;

    Render_ResetGteScreenOffset();
    g_BattleAttackAnimFrame++;
}
