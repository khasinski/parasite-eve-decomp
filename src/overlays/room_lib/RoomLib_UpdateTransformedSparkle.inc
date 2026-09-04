#include "common.h"
#include "pe1/room_fx.h"

extern RoomFxSeed8 D_8018F01C;

RoomFxTransformOwner **func_800C2B50(void);
void ApplyMatrixSV(void *matrix, RoomFxSeed8 *seed, unsigned short *out);
int rand(void);

void ROOMLIB_UPDATE_TRANSFORMED_SPARKLE_NAME(
    void *unused, void *unusedControl, RoomFxPairedSpriteState *state) {
    RoomFxTransformOwner **ownerPtr;
    RoomFxTransform *transform;
    RoomFxSeed8 seed;
    RoomFxSeed8 transformed;

    ownerPtr = func_800C2B50();
    transform = &(*ownerPtr)->transforms[state->transformIndex];
    seed = D_8018F01C;
    ApplyMatrixSV(transform, &seed, (unsigned short *)&transformed);

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

    if ((rand() % 400) == 0) {
        state->sparkleX = *(unsigned short *)&transformed.bytes[0] +
                          transform->x + (rand() % 60) - 30;
        state->sparkleY = *(unsigned short *)&transformed.bytes[2] +
                          transform->y;
        {
            int random;
            int z;
            int transformZ;
            int sign;

            random = rand();
            transformZ = transform->z;
            z = *(unsigned short *)&transformed.bytes[4];
            asm("" : "=r"(transformZ), "=r"(z)
                   : "0"(transformZ), "1"(z));
            state->sparkleAlpha = 0xFF;
            state->sparkleLife = 20;
            sign = random >> 31;
            asm("" : "=r"(z)
                   : "0"(z), "m"(state->sparkleAlpha),
                     "m"(state->sparkleLife), "r"(sign));
            z += transformZ;
            random %= 60;
            state->sparkleZ = z + random - 30;
        }
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

    state->height += (rand() % 30) - 15;
    if (state->height < state->minimumHeight) {
        state->height = state->minimumHeight;
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

#undef ROOMLIB_UPDATE_TRANSFORMED_SPARKLE_NAME
