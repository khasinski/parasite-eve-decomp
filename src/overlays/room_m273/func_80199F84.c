#include "pe1/render_object.h"

typedef struct {
    s16 x, y, z, pad;
} RoomM273Vector;

typedef struct {
    signed int low : 16;
    signed int high : 16;
} RoomM273TrigEntry;

typedef struct {
    char pad_00[4];
    s32 *source;
    s16 scaleX;
    s16 scaleY;
    u16 value;
    u16 step;
} RoomM273PulseState;

extern RoomM273TrigEntry D_800966EC[];
extern char D_8019AB70[];

int func_80199F84(int mode, RoomM273PulseState *state) {
    RoomM273Vector position, rotation;
    s32 frame;
    s32 *source;
    s32 positionX;
    int scaleX, scaleY;
    char *texture;

    if (mode == 1) {
        if (D_800E27EC >= 9) return 1;
        state->value += state->step;
    } else if (mode == 2) {
        source = state->source;
        positionX = source[0];
        position.y = source[1];
        position.z = source[2];
        rotation.x = 0;
        rotation.y = state->value;
        rotation.z = 0;
        frame = D_800E27EC - 1;
        scaleX = state->scaleX;
        scaleY = state->scaleY;
        texture = D_8019AB70;
        position.x = positionX;
        func_800D0E88(&position, &rotation, scaleX, scaleY, state,
                       texture, texture,
                       ((RoomM273TrigEntry *)((char *)D_800966EC +
                        ((frame << 9) & 0x3E00)))->high >> 5, 1);
    }
    return 0;
}
