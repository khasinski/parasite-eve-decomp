/* MASPSX_FLAGS: --expand-div */
#include "common.h"
#include "pe1/psyq_gpu.h"
#include "pe1/render_object.h"

/* Field colour animation: colour-key interpolation and the rotating
 * 16-entry CLUT row upload. */

extern u16 D_800F336C;
extern u16 D_800E1204[];
extern u16 D_800E21A8[];

int StoreImage(RECT *rect, void *pixels);
int LoadImage(RECT *rect, void *pixels);

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

void func_800CF4B4(int arg0, int arg1, u16 *pixels) {
    RECT rect;
    int i;
    u16 *dst;
    int magic;
    int source_offset;

    rect.x = (arg0 & 0xF) << 4;
    rect.y = D_800E1204[D_800F336C] + (arg0 / 16);
    rect.w = 0x10;
    rect.h = 1;

    if (arg1 == -1) {
        StoreImage(&rect, pixels);
        return;
    }

    i = 1;
    magic = 0x88888889;
    dst = D_800E21A8;
    dst[0] = pixels[0];
    do {
        source_offset = (u16)i * 2;
        i++;
        dst[(arg1 % 15) + 1] = *(u16 *)(source_offset + (int)pixels);
        arg1++;
    } while ((unsigned int)(u16)i < 0x10U);

    LoadImage(&rect, dst);
}
