/* MASPSX_FLAGS: --expand-div */
#include "pe1/render_object.h"

void func_800CF3AC(void *key, void *color, int time)
{
    RenderColorTrack *track = (RenderColorTrack *)key;
    /* Matching debt: retain the retail count register across initialization. */
    register int count asm("$8") = track->count;
    int i, total, length;
    key = (unsigned char *)key + 8;
    if (!track->duration) {
        count = 0;
        total = 0;
        /* Walk the timing halfwords; the preceding byte is the encoded
         * duration. Retaining this cursor reproduces the retail accesses. */
        key = (unsigned char *)key + 4;
        for (;;) {
            length = ((unsigned char *)key)[-1];
            if (length) {
                ((RenderColorKeyTiming *)key)->start = total;
                total += length;
                ++count;
                ((RenderColorKeyTiming *)key)->length = length;
                key = (unsigned char *)key + 8;
            } else {
                key = (unsigned short *)(track + 1);
                track->duration = total;
                track->count = count;
                break;
            }
        }
    }
    if (time > track->duration) time = track->duration;
    key = (unsigned char *)key + (count * 8 - 8);
    for (i = 0; i < count; ++i) {
        if (time >= ((RenderColorKey *)key)->timing.start) break;
        key = (unsigned char *)key - 8;
    }
    total = ((RenderColorKey *)key)->timing.length;
    length = time - ((RenderColorKey *)key)->timing.start;
    length = (length << 12) / total;
    LoadAverageCol(key, (RenderColorKey *)key + 1, 4096 - length, length, color);
}
