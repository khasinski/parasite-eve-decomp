#ifndef PE1_SCENE_E19_2_TWIN_GLOW_H
#define PE1_SCENE_E19_2_TWIN_GLOW_H

#include "pe1/render_object.h"
#include "pe1/gte.h"
#include "pe1/room_orbit_trail.h"

/* Scene e19_2 twin glow controller: glows on two actor joints that pulse,
 * burst into a ring of sparks at frame 15 and then fade while a ring and a
 * floor glow spread from the first joint. */
typedef struct SceneTwinGlow {
    /* 0x00 */ s16 state;
    /* 0x02 */ s16 timer;
    /* 0x04 */ s16 jointA;
    /* 0x06 */ s16 jointB;
    /* 0x08 */ GteShortVector position;
} SceneTwinGlow;

typedef struct SceneTwinGlowChannel {
    s32 reserved[2];
    char *pool; /* 0x08 */
} SceneTwinGlowChannel;

/* Floor height read as a one-field record (lhu). */
typedef struct SceneTwinGlowFloor {
    u16 count;
} SceneTwinGlowFloor;

extern GteRotation D_8018F1D4;
extern GteShortVector D_8018F264;
extern RenderColor D_8018F26C;
/* Texture page index read as a one-field record so the read stays behind
 * the first parameter block store through its base register. */
typedef struct SceneTwinGlowPageIndex {
    u16 value;
} SceneTwinGlowPageIndex;

extern SceneTwinGlowPageIndex D_800E11EA;
extern SceneTwinGlowChannel *D_800F32D0, *D_800F33E0;
extern SceneTwinGlowFloor D_800942EC;

int func_80077CF4(int angle);
int func_80077DC4(int angle);
u16 func_80077AA4(int x, int y);
int func_80071A54(void);
int func_800CE560(void *pool, int size, int count, void *callback);
RoomOrbitTrailParticle *func_800CE610(void *pool);
void func_800CE8F0(void *pool, int index, GteShortVector *offset, GteShortVector *position);
int func_80199C28(int mode, void *spark);

#endif
