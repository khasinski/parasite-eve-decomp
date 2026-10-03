/* CC1_FLAGS: -ffixed-22 -ffixed-23 */
#include "pe1/room_m256_trail.h"
#include "pe1/gte.h"

/* Matching debt: keep the division and GTE load in separate allocation
 * blocks. Stock GCC 2.7.2 later merges these identical empty branches;
 * no branch or CPU instruction is emitted by this helper.
 */
static __inline__ void match_reload_boundary(int condition) {
    if (condition)
        asm volatile("");
    else
        asm volatile("");
}
s32 func_801940B0(int mode, void *effect) {
    RoomM256TrailEffect *state = effect;
    RenderColor color = ({ asm volatile("" ::: "$16", "$17", "$18", "$19", "$21"); D_8018F218; });
    RenderColor darkColor = D_8018F21C;
    GteRotation initialRotation = D_8018F220;
    GteRotation spriteRotation = D_8018F1CC;
    RoomM256TrailEffect *particle;
    GteShortVector delta;
    GteShortVector angles;
    GteMatrix transform;
    GteVector scale;
    /* Matching-only space; the retail frame is 0xF0 bytes. */
    volatile char matchingStackReserve[24];
    s16 impactFrame;
    s16 historyIndexX;
    s16 historyIndexY;
    s16 historyIndexZ;
    s32 nextModelFrame;
    s32 nextSpriteFrame;
    s16 renderKind;
    s16 ending;
    s16 turnAngle;
    s16 speedBeforeStep;
    s16 speedAfterStep;
    s16 nextEnding;
    s16 updateKind;
    FieldActorState *trailActorState;
    FieldActorState *trailActorFlags;
    FieldActorState *impactActorState;
    FieldActorState *impactActorFlags;
    s32 impactScale;
    s32 randomVelocityX;
    s32 randomVelocityY;
    s32 modelPage;
    s32 pulseCosine;
    s32 impactCosine;
    s32 randomVelocityZ;
    s32 turnBlend;
    s32 squaredSpeed;
    s32 alive;
    RenderEffectParameters *params;
    RoomM256TrailPalettes *palettes;
    u16 *paletteIndex;
    s16 *floorHeight;
    int tpage;
    int clut;
    s16 impactY;
    int **matrixSlot;
    register int historyCount asm("$5");
    register void *historyPosition asm("$6");
    char *historySample;
    s16 *spriteFrame;
    register int pulseExtent asm("$3");
    register int spriteExtent asm("$6");
    unsigned spritePalette;
    int spriteClutY;
    int translationZ;
    int renderPalette;
    s32 random;
    s32 renderScale;
    s32 fade;
    register int firstFade asm("$21");
    u16 initialPitch;
    u16 initialYaw;
    int trailClutY;
    int pulseClutY;
    int impactClutY;
    int ringClutY;
    int modelClutY;
    u32 tripledScale;
    u32 pulseSine;
    void *motionAngles;
    void *history;
    FieldActorState *trailPlayerState;
    FieldActorState *impactPlayerState;

    switch (mode) {
    case 1:
        updateKind = (s16) state->kind;
        switch (updateKind) {
        case 0:
            state->frame = 0;
            state->kind = 1U;
            state->ending = 0;
            func_800CE8F0(D_800F32D0->actor, 2, &initialRotation, state);
            func_800CE9D4(D_800F32D0->actor, 0, &angles.x);
            historyCount = 16;
            historyPosition = state;
            initialPitch = angles.x + 0x200;
            angles.x = initialPitch;
            {
                int y = (u16)angles.y - 0x200;
                int shiftedIndex = state->index << 9;
                asm volatile("" : : "r"(shiftedIndex), "r"(y), "r"(historyCount), "r"(historyPosition));
                initialYaw = y + shiftedIndex;
            }
            angles.y = initialYaw;
            state->motionX = initialPitch;
            state->motionY = angles.y;
            state->motionZ = angles.z;
            state->motionY = (u16) (state->motionY + 0x800);
            func_800D3AFC((state->index * 0x88) + D_80195EFC, historyCount, historyPosition, 1);
            state->speed = (s16) ((func_80071A54() & 7) + 8);
            state->angle = 0;
            particle = func_800CE610(D_800F33E0->end);

            if (particle != 0) {
                particle->x = (u16) state->x;
                particle->y = (u16) state->y;
                asm volatile("" ::: "memory");
                {
                    int z = (u16)state->z;
                    particle->kind = 4;
                    particle->frame = 0;
                    particle->z = z;
                }

            }
            goto done;
        case 1:
            state->frame = (s16) ((u16) state->frame + 1);
            history = (state->index * 0x88) + D_80195EFC;
            func_800CE870(D_8009D254, 1, &delta.x);
            func_800CFAA8(state, &delta.x, &angles.x);
            ending = state->ending;
            if (ending == 0) {
                if (state->speed >= 0x1B) {
                    turnAngle = state->angle;
                    if (turnAngle < 0x800) {
                        state->angle = (s16) (turnAngle + 0x50);
                    }
                    if (state->angle >= 0x801) {
                        state->angle = 0x800;
                    }
                }
                turnBlend = func_80077CF4(state->angle) * D_80195EF0;
                if (turnBlend < 0) {
                    turnBlend += 0xFFF;
                }
                motionAngles = &state->motionX;
                func_800CFD50(&angles.x, motionAngles, (turnBlend >> 0xC) & 0xFFFF);
                speedBeforeStep = state->speed;
                if (speedBeforeStep < 0x32) {
                    state->speed = (s16) (speedBeforeStep + 6);
                }
                speedAfterStep = state->speed;
                squaredSpeed = speedAfterStep * speedAfterStep;
                if (squaredSpeed < 0) {
                    squaredSpeed += 0xF;
                }
                func_800CFB7C(motionAngles, (s32) (squaredSpeed << 0xC) >> 0x10, &delta.x);
                state->x = (s16) ((u16) state->x + delta.x);
                state->y = (s16) ((u16) state->y + delta.y);
                state->z = (s16) ((u16) state->z + delta.z);
                goto updateHistory;
            }
            nextEnding = ending + 1;
            state->ending = nextEnding;
            if (nextEnding >= 0x10) return 1;
            if (nextEnding < 0x10) {
updateHistory:
                func_800D3AFC(history, 0x10, state, 0);
                if ((func_800C6B90(state, 0x64) != 0) && (state->y >= (D_800942EC - 0x202)) && (D_800E2368->flags != 0) && ((D_800F32D0->actor->state->core_flags & 0x3F000000) == 0x01000000)) {
                    trailPlayerState = D_8009D254->state;
                    trailPlayerState->flags = (s32) (trailPlayerState->flags | 0x4000);
                    trailActorState = D_800F32D0->actor->state;
                    trailActorState->core_flags = (trailActorState->core_flags & 0xC0FFFFFF) | 0x19000000;
                    trailActorFlags = D_800F32D0->actor->state;
                    trailActorFlags->core_flags |= 0x80000000;
                }
                if (state->ending == 0) {
                    if (D_800E27EC == (((D_800E27EC / 3)) * 3)) {
                        particle = func_800CE610(D_800F33E0->end);
                        if (particle != 0) {
                            particle->x = (u16) state->x;
                            particle->y = (u16) state->y;
                            particle->z = (u16) state->z;
                            particle->motionX = (u16) state->motionX;
                            particle->motionY = (u16) state->motionY;
                            asm volatile("" ::: "memory");
                            {
                                int x = (u16)particle->motionX;
                                int z = (u16)state->motionZ;
                                particle->frame = 0;
                                particle->motionX = x + 0x800;
                                asm volatile("" ::: "memory");
                                particle->kind = 3;
                                particle->motionZ = z;
                            }
                        }
                    }
                    particle = func_800CE610(D_800F33E0->end);
                    if (particle != 0) {
                        random = func_80071A54();
                        historyIndexX = state->index;
                        particle->x = (u16) *(s16 *)(D_80195F1C + (((historyIndexX * 0x11) + (random & 3)) * 8));
                        random = func_80071A54();
                        historyIndexY = state->index;
                        particle->y = (u16) *(s16 *)(D_80195F1E + (((historyIndexY * 0x11) + (random & 3)) * 8));
                        random = func_80071A54();
                        historyIndexZ = state->index;
                        particle->z = (u16) *(s16 *)(D_80195F20 + (((historyIndexZ * 0x11) + (random & 3)) * 8));
                        particle->x = (u16) (particle->x - 0x40 + (func_80071A54() & 0x7F));
                        particle->y = (u16) (particle->y - 0x40 + (func_80071A54() & 0x7F));
                        particle->z = (u16) (particle->z - 0x40 + (func_80071A54() & 0x7F));
                        randomVelocityX = func_80071A54();
                        particle->motionX = (s16) (((randomVelocityX % 60)) - 0x1E);
                        randomVelocityY = func_80071A54();
                        particle->motionY = (s16) (((randomVelocityY % 60)) - 0x1E);
                        randomVelocityZ = func_80071A54();
                        particle->kind = 2;
                        particle->frame = 0;
                        particle->motionZ = (s16) (((randomVelocityZ % 60)) - 30);
                    }
                }
                floorHeight = &D_800942EC;
                asm volatile("" : "=r"(floorHeight) : "0"(floorHeight));
                if ((state->y >= *floorHeight) || (((D_800E27EC < 0x51) == 0))) {

                    if (state->ending == 0) {
                        particle = func_800CE610(D_800F33E0->end);
                        if (particle != 0) {
                            particle->x = (u16) state->x;
                            particle->y = (u16) state->y;
                            particle->z = (u16) state->z;
                            impactY = *floorHeight;
                            particle->kind = 5;
                            particle->frame = 0;
                            particle->y = (u16) impactY;
                        }
                        state->ending = 1;
                        goto done;
                    }
                }
            }

            goto done;
        case 2:
            {
                register unsigned frame asm("$3") = *(volatile u16 *)&state->frame;
                register int vx asm("$6") = (s16)state->motionX;
                register unsigned x asm("$2");
                register unsigned vxBits asm("$5");
                register int dampX asm("$6");
                register unsigned y asm("$2"); unsigned z;
                unsigned vy;
                unsigned vz;
                int dampZ;
                asm volatile("" : : "r"(frame), "r"(vx) : "memory");
                x = *(volatile u16 *)&state->x;
                vxBits = *(volatile u16 *)&state->motionX;
                asm volatile("" : : "r"(frame), "r"(vx), "r"(x), "r"(vxBits));
                dampX = vx * 63;
                state->x = x + vxBits;
                asm volatile("" : : : "memory");
                y = *(volatile u16 *)&state->y;
                vy = *(volatile u16 *)&state->motionY;
                state->frame = frame + 1;
                z = (u16)state->z;
                vz = (u16)state->motionZ;
                state->y = y + vy;
                state->z = z + vz;
                state->motionX = dampX / 64;
                dampZ = (s16)state->motionZ * 63;
                if (dampZ < 0) dampZ += 63;
                asm volatile("" : "=r"(dampZ) : "0"(dampZ) : "memory");
                vy = (u16)state->motionY;
                state->motionZ = dampZ >> 6;
                asm volatile("" ::: "memory");
                { int positionY;
                positionY = state->y;
                asm volatile("" : "=r"(positionY) : "0"(positionY));
                state->motionY = vy;
                asm volatile("" ::: "memory");
                if (positionY >= D_800942EC) {
                    int signedY = (s16)vy;
                    asm volatile("" : "=r"(signedY) : "0"(signedY));
                    state->motionY = -signedY;
                }
            }
                }
            alive = state->frame < 0x10;
            goto checkLifetime;
        case 3:
            nextModelFrame = (u16) state->frame + 1;
            state->frame = nextModelFrame;
            asm volatile("" : "=r"(nextModelFrame) : "0"(nextModelFrame), "m"(state->frame));
            alive = (s16)nextModelFrame < 0xA;
            goto checkLifetime;
        case 4:
            nextSpriteFrame = (u16) state->frame + 1;
            state->frame = nextSpriteFrame;
            asm volatile("" : "=r"(nextSpriteFrame) : "0"(nextSpriteFrame), "m"(state->frame));
            alive = (s16)nextSpriteFrame < 0x10;
            goto checkLifetime;
        case 5:
            state->frame = (s16) ((u16) state->frame + 1);
            if ((func_800C6B90(state, 0xB4) != 0) && (state->frame < 0x10) && (D_800E2368->flags != 0) && ((D_800F32D0->actor->state->core_flags & 0x3F000000) == 0x01000000)) {
                impactPlayerState = D_8009D254->state;
                impactPlayerState->flags = (s32) (impactPlayerState->flags | 0x4000);
                impactActorState = D_800F32D0->actor->state;
                impactActorState->core_flags = (impactActorState->core_flags & 0xC0FFFFFF) | 0x19000000;
                impactActorFlags = D_800F32D0->actor->state;
                impactActorFlags->core_flags |= 0x80000000;
            }
            alive = state->frame < 0x18;
checkLifetime:

            if (alive == 0) {
                return 1;
            }

            goto done;
        }
        break;
    case 2:
        renderKind = state->kind - 1;
        switch (renderKind) {
        case 0:
            {
                int *matrix;
                int **slot = &D_800BCFA4.value;
                register int w0 asm("$12");
                register int w1 asm("$13");
                register int w2 asm("$14");
                asm volatile("" : "=r"(slot) : "0"(slot));
                matrix = *slot;
                asm volatile("" : "=r"(matrix) : "0"(matrix) : "$2", "$3", "$4", "$5", "$6", "$7", "$8", "$10", "$11", "$12", "$13", "$14", "$15", "$24", "$25");
                w0 = matrix[0]; w1 = matrix[1];
                gte_ctc2_0(w0); gte_ctc2_1(w1);
                w0 = matrix[2]; w1 = matrix[3]; w2 = matrix[4];
                gte_ctc2_2(w0); gte_ctc2_3(w1); gte_ctc2_4(w2);
                w0 = matrix[5]; w1 = matrix[6];
                gte_ctc2_5(w0);
                w2 = matrix[7];
                gte_ctc2_6(w1); gte_ctc2_7(w2);
            }
            trailClutY = D_800E1204[D_800F336C];
            if ((D_800F336C == 4) && (D_800F3428 != 0)) {
                trailClutY += 4;
            }
            func_800D3114((state->index * 0x88) + D_80195EFC, 0x10, 0xF0, 0, 0x60, 0xFF, 0xF, (s32) D_800E2850[D_800E11EA], func_80077AA4(0x30, trailClutY) & 0xFFFF, 0x80, &color.r, &darkColor.r, 1);
            fade = ((D_800E27EC & 1) << 5) + 0x60;
            func_800D004C(state, 0x190, 0x190, 8, 0, 0x1000, 0x1000, &color.r, 0, fade, 1);
            historySample = D_80195EFC + ((func_80071A54() & 7) * 8);
            func_800D004C((void *)((unsigned)(state->index * 0x88) + (unsigned)historySample), 0x1F4, 0x1F4, 6, 0, 0x1000, 0x1000, &color.r, 0, (u32)fade >> 1, 1);
            goto done;
        case 1:
            firstFade = func_80077DC4(state->frame << 6) / 32;
            renderScale = func_80077DC4(state->frame << 6);
            asm volatile("" ::: "memory");
            params = &D_800F3368;
            spriteExtent = 0x10;
            spriteClutY = D_800E1208;
            asm("" : "=r"(params), "=r"(spriteExtent), "=r"(spriteClutY) : "0"(params), "1"(spriteExtent), "2"(spriteClutY));
            palettes = &D_800E11E8;
            asm("" : "=r"(palettes) : "0"(palettes));
            params->parameter00 = spriteExtent;
            spritePalette = palettes->first;
            D_800F336A = 1;
            D_800F3376 = spriteExtent;
            D_800F3378 = spriteExtent;
            asm volatile("" : "=r"(spritePalette) : "0"(spritePalette), "m"(D_800F3378));
            D_800F336C = 2;
            D_800F336E = 0;
            D_800F3370 = D_800E2850[spritePalette];
            {
                register RoomM256TrailEffect *callState asm("$4");
                register void *callRotation asm("$5");
                register int callScaleX asm("$6");
                register int callScaleY asm("$7");
                int drawClut;
                int drawOffset;

                drawClut = func_80077AA4(0x20, spriteClutY) & 0xFFFF;
                callState = state;
                callRotation = 0;
                callScaleX = renderScale;
                callScaleY = renderScale;
                drawOffset = (s16)((const RenderEffectParameters *)params)->parameter02 * 2 + 0xD8;
                /* Matching debt: consume CLUT and page in $2, then bound both
                 * outgoing stores before the texture-offset load. Empty ASM
                 * only; $4..$7 preserve the ready register arguments. */
                func_800CEE20(callState, callRotation, callScaleX, callScaleY,
                              drawOffset,
                    ({
                        register int clutWord asm("$2") = drawClut;
                        asm("" : "=r"(clutWord) : "0"(clutWord));
                        clutWord;
                    }),
                    ({
                        register int modeWord asm("$2") = 1;
                        asm("" : : "r"(modeWord));
                        modeWord;
                    }), firstFade,
                    ({
                        asm("" : : "r"(callState), "r"(callRotation),
                            "r"(callScaleX), "r"(callScaleY) : "$21", "$2");
                        &color.r;
                    }));
            }
            renderPalette = D_800E2850[palettes->second];
            asm("" : "=r"(renderPalette) : "0"(renderPalette) : "memory");
            D_800F336C = 3;
            D_800F336E = 0;
            D_800F3370 = renderPalette;
            goto done;
        case 2:
            renderScale = func_80077CF4(((state->frame << 0xA) / 10)) / 4;
            fade = (func_80077CF4((state->frame << 0xB) / 10) / 48);
            match_reload_boundary(fade);
            matrixSlot = &D_800BCFA4.value;
            {
                int *matrix;
                int **slot = matrixSlot;
                register int w0 asm("$12");
                register int w1 asm("$13");
                register int w2 asm("$14");
                asm volatile("" : "=r"(slot) : "0"(slot));
                matrix = *slot;
                asm volatile("" : "=r"(matrix) : "0"(matrix) : "$2", "$3", "$4", "$5", "$6", "$7", "$8", "$10", "$11", "$12", "$13", "$14", "$15", "$24", "$25");
                w0 = matrix[0]; w1 = matrix[1];
                gte_ctc2_0(w0); gte_ctc2_1(w1);
                w0 = matrix[2]; w1 = matrix[3]; w2 = matrix[4];
                gte_ctc2_2(w0); gte_ctc2_3(w1); gte_ctc2_4(w2);
                w0 = matrix[5]; w1 = matrix[6];
                gte_ctc2_5(w0);
                w2 = matrix[7];
                gte_ctc2_6(w1); gte_ctc2_7(w2);
            }
            {
                register int pageDepth asm("$4") = 0;
                int pageBlend = 1;
                int pageX = 0;
                asm volatile("" : "=r"(pageDepth), "=r"(pageBlend), "=r"(pageX) : "0"(pageDepth), "1"(pageBlend), "2"(pageX));
            paletteIndex = &D_800E11EA;
            asm volatile("" : "=r"(paletteIndex) : "0"(paletteIndex));
            renderPalette = D_800E2850[*paletteIndex];
            asm volatile("" : "=r"(renderPalette) : "0"(renderPalette));
            D_800F336C = 3;
            asm volatile("" : : "r"(renderPalette) : "memory");
            D_800F336E = 0;
            D_800F3370 = renderPalette;
            tpage = func_80077A64(pageDepth, pageBlend, pageX, 0);
            }
            modelPage = (*(u16 *)((char *)D_800E2850 + ((unsigned)*paletteIndex << 1)) | tpage) & 0xFFFF;
            modelClutY = D_800E1204[D_800F336C];
            if (D_800F336C == 4) {
                if (D_800F3428 != 0) {
                    modelClutY += 4;
                }
            }
            func_800C6EC0(modelPage, func_80077AA4(0x70, modelClutY) & 0xFFFF);
            func_800C6ED8(1);
            func_80079754(&state->motionX, &transform);
            transform.t[0] = (s32) state->x;
            transform.t[1] = (s32) state->y;
            translationZ = state->z;
            scale.x = renderScale;
            scale.y = renderScale;
            scale.z = 0x2AA;
            transform.t[2] = translationZ;
            func_80078CC4(&transform, &scale.x);
            func_800C6EF8(D_80195EF8);
            func_800C7098(D_80195EF8, darkColor.r, darkColor.g, darkColor.b);
            func_800C6FA0(D_80195EF8, fade & 0xFFFF);
            func_800C71E4(D_80195EF8, &transform);
            func_800C6F4C(D_80195EF8);
            goto done;
        case 3:
            pulseSine = func_80077CF4(state->frame << 6);
            renderScale = ((s32) ((pulseSine >> 0x1F) + pulseSine) >> 1) + 0x1000;
            pulseCosine = func_80077DC4(state->frame << 6);
            fade = pulseCosine >> 5;
            if (pulseCosine < 0) {
                fade = (s32) (pulseCosine + 0x1F) >> 5;
            }
            pulseExtent = 0x40;
            asm volatile("" : "=r"(pulseExtent) : "0"(pulseExtent));
            D_800F3368.parameter00 = pulseExtent;
            D_800F3376 = pulseExtent;
            D_800F3378 = pulseExtent;
            asm volatile("" ::: "memory");
            D_800F336A = 4;
            D_800F3374 = 0x18;
            pulseClutY = D_800E1204[D_800F336C];
            if ((D_800F336C == 4) && (D_800F3428 != 0)) {
                pulseClutY += 4;
            }
            func_800CEE20(state, 0, renderScale, renderScale, 0x7C, func_80077AA4(0x60, pulseClutY) & 0xFFFF, 1, fade, 0);
            D_800F3374 = 8;
            goto done;
        case 4:
            renderScale = func_80077CF4(state->frame << 6) + 0x1000;
            func_80077CF4((state->frame << 11) / 24);
            D_800F3368.parameter00 = 0x20;
            D_800F336A = 2;
            D_800F3376 = 0x20;
            D_800F3378 = 0x20;
            impactClutY = D_800E1204[D_800F336C];
            if ((D_800F336C == 4) && (D_800F3428 != 0)) {
                impactClutY += 4;
            }
            clut = func_80077AA4(0x10, impactClutY) & 0xFFFF;
            tripledScale = renderScale * 3;
            impactScale = (s32) (tripledScale + (tripledScale >> 0x1F)) >> 1;
            spriteFrame = &D_800F336A;
            func_800CEE20(state, &spriteRotation, impactScale, impactScale, (*spriteFrame * (s16) (state->frame / 3)) + 0x20, clut, 1, 0x80, 0);
            impactFrame = state->frame;
            if (impactFrame < 0xD) {
                renderScale = func_80077CF4(((impactFrame << 10) / 12)) * 4;
                impactCosine = func_80077DC4(((state->frame << 10) / 12));
                fade = impactCosine >> 5;
                if (impactCosine < 0) {
                    fade = (s32) (impactCosine + 0x1F) >> 5;
                }
                D_800F3368.parameter00 = 0x20;
                *spriteFrame = 2;
                D_800F3376 = 0x20;
                D_800F3378 = 0x20;
                D_800F3372 = 5;
                ringClutY = D_800E1204[D_800F336C];
                if ((D_800F336C == 4) && (D_800F3428 != 0)) {
                    ringClutY += 4;
                }
                func_800CEE20(state, &spriteRotation, renderScale, renderScale, 0x92, func_80077AA4(0x80, ringClutY) & 0xFFFF, 1, fade, &darkColor.r);
                D_800F3372 = 0;
            }
            goto done;
        }
        break;
    default:
        goto done;
    }
done:
    return 0;
}
