#ifndef PE1_ROOM_GLINT_RIBBON_H
#define PE1_ROOM_GLINT_RIBBON_H

#include "pe1/room_spark.h"

/* Glint ribbon controller state: two light ribbons strung across three
 * actor joints that shed glint sparks from the last joint and its floor
 * shadow, then fade out. */
typedef struct RoomGlintRibbon {
    s16 reserved00;               /* 0x00 */
    s16 timer;                    /* 0x02 */
    s16 alpha;                    /* 0x04 */
    s16 reserved06;               /* 0x06 */
    u8 trailA[0xB0];              /* 0x08 */
    u8 trailB[0xB0];              /* 0xB8 */
} RoomGlintRibbon;

PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomGlintRibbon, trailB) == 0xB8,
                  room_glint_ribbon_trail_b);

extern u16 D_800E11E8;
extern void func_800D1384(void *from, void *to, int width, void *color0,
                          void *color1, int alpha, void *trail, int mode);

#endif
