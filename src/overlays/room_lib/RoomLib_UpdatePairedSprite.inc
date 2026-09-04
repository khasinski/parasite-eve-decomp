#include "common.h"
#include "pe1/room_fx.h"

RoomFxTransformOwner **func_800C2B50(void);
int rand(void);

void ROOMLIB_UPDATE_PAIRED_SPRITE_NAME(void *unused,
                                       RoomFxControl *control,
                                       RoomFxPairedSpriteState *state) {
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

    if ((control->frame % 30) == 0) {
        state->x = transform->x;
        state->y = transform->y;
        state->z = transform->z;
        state->velocityX = 0;
        state->velocityY = -5;
        state->velocityZ = 0;
        state->alpha = 0x80;
        state->alphaStep = 0x96;
    }

    state->x += state->velocityX;
    state->y += state->velocityY;
    state->z += state->velocityZ;
    state->alphaStep += 60;
    state->alpha -= 4;
    if (state->alpha < 0) {
        state->alpha = 0;
    }

    if ((control->frame % 300) == 0) {
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
        y -= 5;
        state->sparkleAlpha = alpha;
        state->sparkleLife = life;
        y -= random % 20;
        state->sparkleY = y;
    }

    if (state->height < 1000) {
        state->height += rand() % 50;
    }

    if (state->height >= 501) {
        state->height -= rand() % 30;
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
