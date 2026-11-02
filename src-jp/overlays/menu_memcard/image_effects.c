#include "menu_memcard_glint.h"
#include "menu_memcard_link.h"
#include "menu_memcard_pulse.h"
#include "menu_memcard_trail.h"

s32 MenuCard_PulseImage(s32 mode, MemcardPulseState *state) {
    GteRotation rotation;
    RenderColor color;
    s32 angle;
    s32 scale;
    s32 palette;
    register s16 timer;
    register s32 twist;
    register void *track;
    switch (mode) {
    case 1:
        if (state->phase != 0) return 0;
        timer = state->timer + 1;
        state->timer = timer;
        if (timer < 16) return 0;
        __asm__ volatile("" : : : "memory");
        return 1;
    case 2:
        if (state->phase != 0) return 0;
        angle = state->timer << 6;
        scale = rsin(angle) + 2048;
        rcos(angle);
        rotation.x = 0; rotation.y = 0; twist = state->timer;
        track = g_MenuCardPulseColorTrack;
        rotation.flags = 0;
        rotation.z = twist << 5;
        FieldFx_SampleColorTrack(track, &color, state->timer << 1);
        g_FieldFxSpriteParams.parameter00 = 32;
        g_FieldFxSpriteParams.parameter02 = 2;
        g_FieldFxSpriteParams.extent_x = 32;
        g_FieldFxSpriteParams.extent_y = 32;
        palette = g_FieldFxPaletteTable[g_FieldFxSpriteParams.palette];
        if (g_FieldFxSpriteParams.palette == 4 && g_FieldFxAltPaletteFlag != 0) palette += 4;
        FieldFx_DrawShapeQuads(&state->position, &rotation, scale, scale, 130,
                     GetClut(48, palette), 1, 128, &color);
        break;
    }
    return 0;
}

s32 MenuCard_LinkEffect(s32 mode, MemcardLinkState *state) {
    GteShortVector unused;
    GteShortVector first;
    GteShortVector second;
    MemcardJointOffset offset = g_MenuCardHandOffset;
    RenderColor color;
    MemcardLinkColor color1 = g_MenuCardLinkColor1;
    MemcardLinkColor color2 = g_MenuCardLinkColor2;
    register MemcardJointOffset *offsetPtr asm("$6");
    switch (mode) {
    case 0: {
        s32 opacity;
        void *entity;
        register GteShortVector *firstPtr asm("$17");
        offsetPtr = &offset;
        __asm__("" : : "r"(offsetPtr));
        opacity = 128; entity = D_8009CD10;
        __asm__("" : "=r"(opacity) : "0"(opacity) : "$17");
        firstPtr = &first;
        state->opacity = opacity; state->phase = 0; state->timer = 0;
        __asm__("" : : : "memory");
        FieldFx_TransformMatrixPoint(entity, g_MenuCardLinkJointA, offsetPtr, firstPtr);
        FieldFx_TransformMatrixPoint(D_8009CD10, g_MenuCardLinkJointB, &offset, &second);
        FieldFx_DrawHistoryTrail(&first, &second, 1016, (unsigned char *)&color2, (unsigned char *)&color1, 128, (struct FieldTrailPair *)&state->rotation, 1);
        return FieldFx_InitCallbackList(ANIM_POOL_AS(MemcardLinkChannel)->slots, 20, 22,
                             (FieldFxCallbackListCallback)MenuCard_PulseImage);
    }
    case 1:
        {
            s16 timer = state->timer + 1;
            void *entity = D_8009CD10;

            state->timer = timer;
            __asm__("" : : : "memory");
            FieldFx_TransformMatrixPoint(entity, g_MenuCardLinkJointB, &offset, &unused);
        }
        if (state->timer >= 23 && state->opacity > 0) state->opacity -= 8;
        if (state->timer < 22) return 0;
        __asm__ volatile("" : :);
        return 2;
    case 2:
        if (state->opacity >= 0) {
            FieldFx_SampleColorTrack(g_MenuCardPulseColorTrack, &color, state->timer);
            FieldFx_TransformMatrixPoint(D_8009CD10, g_MenuCardLinkJointA, &offset, &first);
            FieldFx_TransformMatrixPoint(D_8009CD10, g_MenuCardLinkJointB, &offset, &second);
            gte_ldrotmatrix(g_ViewMatrixSlot.value);
            gte_ldtransmatrix(g_ViewMatrixSlot.value);
            FieldFx_DrawHistoryTrail(&first, &second, 16, (unsigned char *)&color2, (unsigned char *)&color1,
                         state->opacity, (struct FieldTrailPair *)&state->rotation, 1);
        }
        {
            u16 tpage = g_FieldFxTpageTable[g_FieldFxTpageSlot1];
            g_FieldFxSpriteParams.palette = 1;
            g_FieldFxSpriteParams.tpage = tpage;
        }
        FieldFx_EnsureEffectTexturePage(1);
        g_FieldFxSpriteParams.parameter06 = 0;
        g_FieldFxSpriteParams.parameter0A = 0;
        g_FieldFxSpriteParams.depth = 16;
        break;
    }
    return 0;
}

s32 MenuCard_CosineImage(s32 mode, MemcardPulseState *state) {
    GteRotation rotation;
    s32 angle, scale, palette;
    register s32 intensity;
    register s32 cosine asm("$3");
    register s32 index;
    register s32 four;
    s16 timer;
    s32 twist;
    switch (mode) {
    case 1:
        if (state->phase != 0) return 0;
        timer = state->timer + 1;
        state->timer = timer;
        if (timer < 12) return 0;
        __asm__ volatile("" : : : "memory");
        return 1;
    case 2:
        if (state->phase != 0) return 0;
        angle = (state->timer << 10) / 12;
        scale = rcos(angle) + 2048;
        cosine = rcos(angle);
        __asm__("" : : "r"(cosine));
        intensity = cosine / 32;
        rotation.x = 0; rotation.y = 0;
        twist = state->timer;
        index = g_FieldFxSpriteParams.palette;
        __asm__ volatile("" ::: "memory");
        four = 4;
        __asm__("" : : "r"(four));
        rotation.flags = 0;
        rotation.z = twist << 5;
        palette = g_FieldFxPaletteTable[index];
        if (index == four && g_FieldFxAltPaletteFlag != 0) palette += 7;
        else palette += 3;
        FieldFx_DrawShapeQuads(&state->position, &rotation, scale, scale, 66,
                     GetClut(0, palette), 1, intensity, &g_MemcardCosineColor);
        break;
    }
    return 0;
}

s32 MenuCard_TrailEffect(s32 mode, MemcardTrailState *state) {
    GteShortVector position;
    MemcardJointOffset offset = g_MenuCardHandOffset;
    RenderColor color;
    s32 scale, palette;
    s32 index;
    s32 four;
    switch(mode) {
    case 0:
        state->attachment = 19; state->phase = 0; state->timer = 0;
        return FieldFx_InitCallbackList(ANIM_POOL_AS(MemcardLinkChannel)->slots, 20, 24,
                             (FieldFxCallbackListCallback)MenuCard_CosineImage);
    case 1:
        state->timer++;
        FieldFx_TransformMatrixPoint(D_8009CD10, state->attachment, &offset, &position);
        if (state->timer < 33 && (state->timer & 1)) {
            MemcardPulseState *image = FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(MemcardLinkChannel)->slots);
            if (image) {
                image->position.x = position.x;
                image->position.y = position.y;
                image->position.z = position.z;
                image->phase = 0; image->timer = 0;
            }
        }
        if (state->timer >= 32) return 2;
        break;
    case 2:
        gte_ldrotmatrix(g_ViewMatrixSlot.value);
        gte_ldtransmatrix(g_ViewMatrixSlot.value);
        FieldFx_TransformMatrixPoint(D_8009CD10, state->attachment, &offset, &position);
        g_FieldFxSpriteParams.parameter00 = 32;
        g_FieldFxSpriteParams.parameter02 = 2;
        g_FieldFxSpriteParams.extent_x = 32; g_FieldFxSpriteParams.extent_y = 32;
        {
            u16 tpage = g_FieldFxTpageTable[g_FieldFxTpageSlot9];
            g_FieldFxSpriteParams.palette = 1;
            g_FieldFxSpriteParams.tpage = tpage;
        }
        FieldFx_EnsureEffectTexturePage(1);
        g_FieldFxSpriteParams.parameter06 = 1;
        g_FieldFxSpriteParams.parameter0A = 5;
        g_FieldFxSpriteParams.depth = 32;
        if (state->timer < 33) {
            FieldFx_SampleColorTrack(g_MenuCardTrailColorTrack, &color, state->timer);
            scale = rsin(state->timer << 6) + 4096;
            __asm__("" : "=r"(scale) : "0"(scale) : "memory");
            index = g_FieldFxSpriteParams.palette;

            four = 4;

            palette = g_FieldFxPaletteTable[index];
            if (index == four && g_FieldFxAltPaletteFlag) palette += 8;
            else palette += 4;
            FieldFx_DrawShapeQuads(&position, 0, scale, scale, 64, GetClut(0,palette), 1, 128, &color);
            index = g_FieldFxSpriteParams.palette;
            __asm__("" : "=r"(index) : "0"(index) : "$3", "memory");
            four = 4;

            palette = g_FieldFxPaletteTable[index];
            if (index == four && g_FieldFxAltPaletteFlag) palette += 7;
            else palette += 3;
            FieldFx_DrawShapeQuads(&position, 0, scale, scale, 70, GetClut(0,palette), 1, 128, &color);
            g_MemcardCosineColor.r = color.r; g_MemcardCosineColor.g = color.g; g_MemcardCosineColor.b = color.b;
        } else {
            *(u32 *)&g_MemcardCosineColor = 0;
        }
        g_FieldFxSpriteParams.parameter0A = 0;
        g_FieldFxSpriteParams.depth = 16;
        break;
    }
    return 0;
}

s32 MenuCard_SineRotatingImage(s32 mode, MemcardPulseState *state) {
    GteRotation rotation;
    s32 angle, scale;
    register s32 palette;
    register s32 intensity;
    register s32 cosine asm("$5");
    register s32 index;
    register s32 four asm("$3");
    s16 timer;
    register s32 twist;
    switch (mode) {
    case 1:
        if (state->phase != 0) return 0;
        timer = state->timer + 1;
        state->timer = timer;
        if (timer < 12) return 0;
        __asm__ volatile("" : : : "memory");
        return 1;
    case 2:
        if (state->phase != 0) return 0;
        angle = (state->timer << 10) / 12;
        scale = rsin(angle) + 4096;
        cosine = rcos(angle);
        if (cosine < 0) cosine += 31;
        rotation.x = state->rotation.x;
        index = g_FieldFxSpriteParams.palette;
        rotation.y = state->rotation.y;
        rotation.z = state->rotation.z;
        intensity = cosine >> 5;
        __asm__ volatile("" ::: "memory");
        twist = state->timer;
        rotation.flags = 1;
        rotation.z = twist * 50;
        {
            four = 4;
            palette = g_FieldFxPaletteTable[index];
        }
        if (index == four && g_FieldFxAltPaletteFlag != 0) palette += 7;
        else palette += 3;
        FieldFx_DrawShapeQuads(&state->position, &rotation, scale, scale, 68,
                     GetClut(0, palette), 1, intensity, &g_MemcardSineColor);
        break;
    }
    return 0;
}

s32 MenuCard_SineTrailEffect(s32 mode, MemcardTrailState *state) {
    GteShortVector position;
    MemcardJointOffset offset = g_MenuCardHandOffset;
    RenderColor color;
    RenderMatrixSlot *matrixSlot;
    s32 scale, kind, palette;

    switch (mode) {
    case 0:
        state->attachment = 25;
        state->phase = 0;
        state->timer = 0;
        return FieldFx_InitCallbackList(ANIM_POOL_AS(MemcardLinkChannel)->slots, 20, 24,
                             (FieldFxCallbackListCallback)MenuCard_SineRotatingImage);
    case 1:
        state->timer++;
        FieldFx_TransformMatrixPoint(D_8009CD10, state->attachment, &offset, &position);
        if (state->timer < 33 && (state->timer & 1)) {
            MemcardPulseState *image = FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(MemcardLinkChannel)->slots);
            if (image) {
                image->position.x = position.x;
                image->position.y = position.y;
                image->position.z = position.z;
                FieldFx_GetTransformLookAngles(D_8009CD10, 0, (GteShortVector *)&image->rotation);
                image->phase = 0;
                image->timer = 0;
            }
        }
        if (state->timer >= 32) {
            return 2;
        }
        break;
    case 2:
        matrixSlot = &g_ViewMatrixSlot;
        gte_ldrotmatrix(matrixSlot->value);
        gte_ldtransmatrix(matrixSlot->value);
        FieldFx_TransformMatrixPoint(D_8009CD10, state->attachment, &offset, &position);
        g_FieldFxSpriteParams.parameter00 = 32;
        g_FieldFxSpriteParams.parameter02 = 2;
        g_FieldFxSpriteParams.extent_x = 32;
        g_FieldFxSpriteParams.extent_y = 32;
        {
            u16 tpage = g_FieldFxTpageTable[g_FieldFxTpageSlot9];
            g_FieldFxSpriteParams.palette = 1;
            g_FieldFxSpriteParams.tpage = tpage;
        }
        FieldFx_EnsureEffectTexturePage(1);
        g_FieldFxSpriteParams.parameter06 = 1;
        g_FieldFxSpriteParams.parameter0A = 5;
        g_FieldFxSpriteParams.depth = 32;
        if (state->timer < 33) {
            FieldFx_SampleColorTrack(g_MenuCardSineTrailColorTrack, &color, state->timer);
            scale = rsin(state->timer << 6) / 2 + 2048;
            kind = g_FieldFxSpriteParams.palette;
            palette = g_FieldFxPaletteTable[kind];
            FieldFx_DrawShapeQuads(&position, 0, scale, scale, 64,
                          GetClut(0, (kind == 4 && g_FieldFxAltPaletteFlag) ? palette + 8 : palette + 4),
                          1, 128, &color);
            g_MemcardSineColor.r = color.r;
            g_MemcardSineColor.g = color.g;
            g_MemcardSineColor.b = color.b;
        } else {
            *(u32 *)&g_MemcardSineColor = 0;
        }
        g_FieldFxSpriteParams.parameter0A = 0;
        g_FieldFxSpriteParams.depth = 16;
        break;
    }
    return 0;
}

/* Origin callbacks of the glint effect descriptors; every effect is
 * anchored at the shared glint origin. */
GteShortVector *MenuCard_GetGlintOriginA(void) {
    return &g_MemcardGlintOrigin;
}

GteShortVector *MenuCard_GetGlintOriginB(void) {
    return &g_MemcardGlintOrigin;
}

GteShortVector *MenuCard_GetGlintOriginC(void) {
    return &g_MemcardGlintOrigin;
}

GteShortVector *MenuCard_GetGlintOriginD(void) {
    return &g_MemcardGlintOrigin;
}

GteShortVector *MenuCard_GetGlintOriginE(void) {
    return &g_MemcardGlintOrigin;
}

GteShortVector *MenuCard_GetGlintOriginF(void) {
    return &g_MemcardGlintOrigin;
}

/* Update hook of the cross flash: keeps it alive while the flash raises the
 * flag each frame and clears the flag for the next frame. */
int MenuCard_ConsumeCrossFlashFlag(int mode) {
    if (mode == 1) {
        if (g_MenuCardCrossFlashRaised == 0) return 1;
        g_MenuCardCrossFlashRaised = 0;
    }
    return 0;
}
