#ifndef PE1_ROOM_M005_H
#define PE1_ROOM_M005_H

#include "pe1/gte_types.h"
#include "pe1/render_object.h"

/* Drifting sprite drawn by the room_m005 effect callback. */
typedef struct RoomM005DriftingSpriteState {
    unsigned short x;             /* 0x00 */
    unsigned short y;             /* 0x02 */
    unsigned short z;             /* 0x04 */
    short pad06;                  /* 0x06 */
    unsigned short vx;            /* 0x08 */
    unsigned short vy;            /* 0x0A */
    unsigned short vz;            /* 0x0C */
} RoomM005DriftingSpriteState;

extern RenderColor D_8018F00C;
extern unsigned short D_80190BA0;
extern unsigned short D_80190BA2;
extern unsigned short D_80190BA4;
extern GteShortVector D_80190BA8;

int func_80077DC4(int angle);
int func_80077AA4(int, int);
int func_8018FB84(int mode, RoomM005DriftingSpriteState *state);

#endif
