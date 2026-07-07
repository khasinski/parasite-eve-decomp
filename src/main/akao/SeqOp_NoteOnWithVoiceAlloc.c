/* CC1_FLAGS: -g3 */
/* MASPSX_FLAGS: -G8 */
#include "pe1/akao.h"

typedef short s16;
typedef int s32;

void SeqOp_SetTrack38Mask();

extern void *g_AkaoCurTrack[];
#define g_AkaoCurTrack (g_AkaoCurTrack[0])

void SeqOp_NoteOnWithVoiceAlloc(AkaoTrack *track, s32 arg1, s32 arg2, s16 arg3) {
    register s16 voice_index asm("$7");
    register s32 voice_mask asm("$6");
    s32 used_mask;
    register s32 valid_mask asm("$8");
    s32 flags;
    unsigned char *cursor;

    voice_mask = arg2;
    voice_index = arg3;
    cursor = track->pc;
    track->pc = cursor + 1;
    flags = track->flags;
    track->pitch_slide_current = *cursor << 8;
    track->pitch_slide_duration = 0;
    if (!(flags & AKAO_TRACK_FLAG_VOICE_ALLOCATED)) {
        voice_index = 0;
        voice_mask = 1;
        valid_mask = 0xFFFFFF;
        used_mask = *(s32 *)(g_AkaoCurTrack + 4) | *(s32 *)(g_AkaoCurTrack + 0x30);
loop_2:
        if (used_mask & voice_mask) {
            voice_mask *= 2;
            voice_index += 1;
            if (!(voice_mask & valid_mask)) {

            } else {
                goto loop_2;
            }
        }
        if (voice_mask & 0xFFFFFF) {
            *(s32 *)(g_AkaoCurTrack + 0x30) |= voice_mask;
            track->voice_index = voice_index;
            track->flags |= AKAO_TRACK_FLAG_VOICE_ALLOCATED;
        }
    }
    SeqOp_SetTrack38Mask();
}
