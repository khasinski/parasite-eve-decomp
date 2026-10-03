#ifndef MENU_MEMCARD_GLINT_H
#define MENU_MEMCARD_GLINT_H

#include "pe1/room_spark.h"
#include "pe1/gte.h"

/* Scene object reached through the primary channel pool. */
typedef struct MemcardGlintAnchor {
    u8 reserved[0x268];
    s16 x, y, z;                  /* 0x268 */
} MemcardGlintAnchor;

typedef struct MemcardGlintBurst {
    s16 x, y, z;
} MemcardGlintBurst;

extern RenderColor D_801ED844;
extern u8 D_801F1CB0[];
extern void func_800CF3AC(void *track, void *color, int time);
extern void func_800D1AE0(RenderColor *color, int intensity, int step, int count);

int Memcard_FadingGlintParticle(int mode, RoomDampedSpark *spark);
int Memcard_GlintBurstController(int mode, MemcardGlintBurst *burst);

#endif
