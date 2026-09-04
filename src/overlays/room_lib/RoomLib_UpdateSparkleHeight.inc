#include "common.h"
#include "pe1/room_fx.h"

RoomFxTransformOwner **func_800C2B50(void);
int rand(void);

void ROOMLIB_UPDATE_SPARKLE_HEIGHT_NAME(
    void *unused, RoomFxControl *control, RoomFxPairedSpriteState *state) {
    RoomFxTransformOwner **ownerPtr;
    RoomFxTransform *transform;

    ownerPtr = func_800C2B50();
    transform = &(*ownerPtr)->transforms[state->transformIndex];

    state->phase++;
    state->phase %= 4;

    if (state->counter < 200) {
        state->counter++;
    }

    if (state->resourceSelector > 0) {
        state->resourceSelector--;
        if (state->resourceSelector == 0) {
            state->active = 0;
        }
    }

    if ((rand() % 200) == 0) {
        state->sparkleX = transform->x;
        state->sparkleY = transform->y;
        state->sparkleZ = transform->z;
        state->sparkleAlpha = 0xFF;
        state->sparkleLife = 20;
    }

    if (state->sparkleLife > 0) {
        int random;
        int life;
        int alpha;
        int y;

        random = rand();
        life = *(unsigned short *)&state->sparkleLife;
        alpha = state->sparkleAlpha;
        y = state->sparkleY;
        life--;
        alpha -= 8;
        y -= 10;
        state->sparkleAlpha = alpha;
        state->sparkleLife = life;
        y -= random % 20;
        state->sparkleY = y;
    }

    if (control->frame >= 31) {
        state->height -= rand() % 20;
        state->height += rand() % 2;
        if (state->height < 500) {
            state->height = 500;
        }
    } else {
        if (state->height < 1300) {
            state->height += rand() % 60;
        }
        if (state->height >= 1101) {
            state->height -= rand() % 40;
        }
    }

    if (state->resourceSelector == -1 && state->width != 0x80) {
        state->width += 8;
    }

    if ((unsigned short)(state->resourceSelector - 1) < 31) {
        state->width -= 4;
        if (state->width < 0) {
            state->width = 0;
        }
    }
}

#undef ROOMLIB_UPDATE_SPARKLE_HEIGHT_NAME
