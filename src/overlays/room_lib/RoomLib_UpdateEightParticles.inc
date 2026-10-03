#include "pe1/field_script_context.h"
#include "common.h"
#include "pe1/gte_types.h"
#include "pe1/room_fx.h"

extern RoomFxSeed8 ROOMLIB_UPDATE_EIGHT_SEED0;
extern RoomFxSeed8 ROOMLIB_UPDATE_EIGHT_SEED1;
extern RoomFxSeed8 ROOMLIB_UPDATE_EIGHT_SEED2;

GteShortVector *ApplyMatrixSV(const GteMatrix *matrix,
                              const GteShortVector *seed,
                              GteShortVector *out);
int rand(void);

void ROOMLIB_UPDATE_EIGHT_NAME(void *unused,
                               RoomEightParticleControl *control,
                               RoomEightParticleState *stateArg) {
    RoomEightParticleControl *clock;
    register RoomEightParticleState *state asm("$19");
    unsigned int i;
    register char *positionCursor asm("$16");
    register char *scalarCursor asm("$17");
    RoomEightParticleContext *context;
    char *matrix;
    char *activeCursor;
    RoomFxSeed8 seed0;
    RoomFxSeed8 seed1;
    RoomFxSeed8 seed2;
    RoomFxSeed8 transformed0;
    RoomFxSeed8 transformed1;
    RoomFxSeed8 transformed2;
    RoomEightParticleContext *contextResult;
    char *matrixBase;
    int frame;
    s16 slot;
    int random;

    clock = control;
    state = stateArg;
    contextResult = func_800C2B50();
    i = 0;
    scalarCursor = (char *)state;
    context = contextResult;
    matrixBase = *(char **)(context->root + 0x238);
    positionCursor = (char *)state;
    seed0 = ROOMLIB_UPDATE_EIGHT_SEED0;
    seed1 = ROOMLIB_UPDATE_EIGHT_SEED1;
    seed2 = ROOMLIB_UPDATE_EIGHT_SEED2;
    matrix = matrixBase + 0xA0;

    do {
        activeCursor = (char *)state + i;
        if (activeCursor[0xB4] == 0) {
            frame = clock->frame;
            slot = frame % 8;
            if (slot == i && frame < 90) {
                activeCursor[0xB4] = 1;
                state->activeCount++;
                ApplyMatrixSV((const GteMatrix *)matrix,
                              (const GteShortVector *)&seed0,
                              (GteShortVector *)&transformed0);
                ApplyMatrixSV((const GteMatrix *)matrix,
                              (const GteShortVector *)&seed1,
                              (GteShortVector *)&transformed1);
                ApplyMatrixSV((const GteMatrix *)matrix,
                              (const GteShortVector *)&seed2,
                              (GteShortVector *)&transformed2);

                *(u16 *)(positionCursor + 4) =
                    *(u16 *)&transformed0.bytes[0] + context->baseX;
                *(u16 *)(positionCursor + 6) =
                    *(u16 *)&transformed0.bytes[2] + context->baseY;
                *(u16 *)(positionCursor + 8) =
                    *(u16 *)&transformed0.bytes[4] + context->baseZ;

                random = rand();
                if ((random & 1) == 0) {
                    *(RoomFxSeed8 *)(positionCursor + 0x44) = transformed2;
                } else {
                    *(RoomFxSeed8 *)(positionCursor + 0x44) = transformed1;
                }
                *(u16 *)(scalarCursor + 0x84) = 0x800;
                *(s16 *)(scalarCursor + 0x94) = 0x80;
                *(u16 *)(scalarCursor + 0xA4) = 0;
            }
        } else {
            int randomValue;
            int yValue;
            int zVelocity;
            int zPosition;
            int zValue;

            *(u16 *)(positionCursor + 4) +=
                *(u16 *)(positionCursor + 0x44);
            randomValue = rand();
            randomValue %= 20;
            zPosition = *(u16 *)(positionCursor + 8);
            zVelocity = *(u16 *)(positionCursor + 0x48);
            asm("" : "=r"(zPosition), "=r"(zVelocity)
                   : "0"(zPosition), "1"(zVelocity));
            yValue = *(u16 *)(positionCursor + 6);
            yValue -= 20;
            zValue = zPosition + zVelocity;
            asm("" : "=r"(yValue), "=r"(zValue)
                   : "0"(yValue), "1"(zValue));
            *(u16 *)(positionCursor + 8) = zValue;
            yValue -= randomValue;
            *(u16 *)(positionCursor + 6) = yValue;
            *(s16 *)(scalarCursor + 0x94) -= 8;
            (*(u16 *)(scalarCursor + 0xA4))++;
            if (*(s16 *)(scalarCursor + 0x94) <= 0) {
                *(s16 *)(scalarCursor + 0x94) = 0;
                activeCursor[0xB4] = 0;
                state->activeCount--;
            }
        }

        scalarCursor += 2;
        i++;
        positionCursor += 8;
    } while (i < 8);

    if (state->activeCount == 0 && clock->frame >= 91) {
        clock->state = 2;
    }
}

#undef ROOMLIB_UPDATE_EIGHT_NAME
#undef ROOMLIB_UPDATE_EIGHT_SEED0
#undef ROOMLIB_UPDATE_EIGHT_SEED1
#undef ROOMLIB_UPDATE_EIGHT_SEED2
