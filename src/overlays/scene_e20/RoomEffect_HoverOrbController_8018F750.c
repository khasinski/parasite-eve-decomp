/* MASPSX_FLAGS: --expand-div */
/* CC1_FLAGS: -ffixed-10 -ffixed-11 -ffixed-15 -ffixed-21 -ffixed-22 -ffixed-23 -ffixed-24 -ffixed-25 */
#include "pe1/gte.h"
#include "pe1/scene_e20_hover_orb.h"
#define NULL ((void *)0)

/* mode 0: initialize; mode 1: advance the effect; mode 2: draw.
 * Pins and empty barriers preserve retail allocation and scheduling.
 * See docs/ASM_AND_GTE_POLICY.md for the matching debt. */
s32 func_8018F750(s32 mode, SceneE20TrailEffect *effect, SceneEffectSlot *command) {
    register s32 initialMode asm("$20");
    register SceneE20Color *initColor0 asm("$17");
    register SceneE20Color *initColor1 asm("$19");
    register u16 headPositionZ asm("$8");
    register u16 headPositionY asm("$7");
    register u16 headOffsetX asm("$2");
    register u16 headPositionX asm("$3");
    SceneE20Vec point;
    SceneE20Vec drawRotation;
    SceneE20Color beamColor;
    SceneE20Color edgeColor;
    SceneE20Color flashA;
    SceneE20Color flashB;
    SceneE20Matrix matrix;
    s32 scale[5]; /* Three scale components plus eight bytes of frame padding. */
    register SceneE20Color *oddColor asm("$8");
    register SceneE20Color *evenColor asm("$6");
    SceneE20Actor *temp_v1_4;
    register SceneE20Color *flashColor asm("$20");
    SceneE20Node *temp_a0;
    SceneE20Node *temp_a0_2;
    SceneE20Node *temp_v1_3;
    SceneE20Particle *temp_v0_10;
    SceneE20Particle *temp_v0_12;
    SceneE20Vec *temp_s0_2;
    SceneE20Vec *var_a1;
    s16 temp_v0_14;
    s16 temp_v0_2;
    s16 temp_v0_5;
    s16 temp_v0_7;
    s16 temp_v0_9;
    s16 temp_v1;
    s16 temp_v1_2;
    s32 var_s0;
    register s32 modelScale asm("$17");
    u32 assetKey;
    register s32 paletteX asm("$4");
    s32 paletteY;
    s32 translationZ;
    s32 drawAngle;
    u32 particleZ, effectZ;
    register u16 endpointX asm("$4");
    s32 temp_ret;
    s32 temp_s0;
    s32 temp_s0_3;
    s32 temp_s3;
    s32 temp_v0_13;
    s32 temp_v0_4;
    s32 var_a0;
    s32 var_a1_2;
    register s32 particleIndex asm("$19");
    s32 var_v0;
    u16 temp_a1;
    u16 texturePage;
    u16 *pageIndex;
    register int pageArg0 asm("$4");
    register int pageArg1 asm("$5");
    register int pageArg2 asm("$6");
    u32 textureIndex;
    u16 temp_v0_3;
    void *temp_v0;

    beamColor = D_8018EFFC;
    edgeColor = D_8018F000;
    flashA = D_8018F004;
    flashB = D_8018F008;
    initColor0 = &beamColor; initColor1 = &edgeColor;
    oddColor = &flashA;
    initialMode = 1;
    if (mode == initialMode) { evenColor = &flashB; goto update; }
    evenColor = &flashB;
    if (mode < 2) {
        if (mode == 0) goto initialize;
        return 0;
    }
    if (mode == 2) goto render;
    return 0;
initialize:
            effect->phase = 0;
            effect->timer = 0;
            effect->alpha = 0x80;
            command->pending = 0;
            effect->target.x = (s16) (u16) D_800F32D0->pool->position.x;
            effect->target.y = (s16) (u16) D_800F32D0->pool->position.y;
            effect->target.z = (s16) (u16) D_800F32D0->pool->position.z;
            assetKey = 0xC5540000U;
            effect->previousPosition.x = (s16) (u16) effect->target.x;
            effect->previousPosition.y = (s16) (u16) effect->target.y;
            effect->previousPosition.z = (s16) (u16) effect->target.z;
            effect->position.x = (s16) (u16) effect->target.x;
            effect->position.y = (s16) (u16) effect->target.y;
            effect->position.z = (s16) (u16) effect->target.z;
            asm volatile("" : "=r"(assetKey) : "0"(assetKey) : "memory");
            temp_v0 = func_8006E498(D_800B0E64, assetKey | 0x3704U);
            D_8019085C = temp_v0;
            func_800C6D5C(temp_v0, 0, 0);
            func_800D1384(&effect->position, &effect->position, 0x3EE, initColor0, initColor1, 0x80, effect->history, initialMode);
            return func_800CE560(D_800F33E0->pool, 0x14, 0x20, func_8018F028);
update: {
    temp_v1_2 = effect->phase;
    switch (temp_v1_2) {                            /* switch 1 */
    case 0:                                         /* switch 1 */
        command->pending = 0;
        func_800CE870(D_8009D254, 0, &point);
        point.x = (func_80071A54() & 0x7FF) - 0x400;
        point.y = ((u16) D_800942EC.height - 0x258) - (func_80071A54() & 0x1FF);
        point.z = (func_80071A54() & 0x7FF) - 0x400;
        effect->target.x = (s16) (u16) point.x;
        effect->target.y = (s16) (u16) point.y;
        effect->target.z = (s16) (u16) point.z;
        func_800CE870(D_8009D254, 1, &effect->endpoint);
        effect->duration = (func_80071A54() & 0xF) + 6;
        effect->timer = 0;
        effect->phase = 1;
        break;
    case 1:                                         /* switch 1 */
        temp_v0_5 = (u16) effect->timer + 1;
        effect->timer = temp_v0_5;
        if ((temp_v0_5 == 1) && (func_80071A54() & 1)) {
            func_8006DCE4(0x5FF, func_800D3FD8(), effect->position.x, effect->position.y, (s32) effect->position.z);
        }
        temp_s3 = func_80077CF4((s32) (effect->timer << 0xA) / (s16) effect->duration);
        func_800783E4(&effect->previousPosition, &effect->target, 0x1000 - temp_s3, temp_s3, &effect->position);
        func_800CE870(D_8009D254, 1, &effect->endpoint);
        if (effect->timer >= effect->duration) {
            effect->phase = 2;
            effect->timer = 0;
        }
        break;
    case 2:                                         /* switch 1 */
        effect->timer = (u16) effect->timer + 1;
        func_800CE870(D_8009D254, 1, &effect->endpoint);
        if (effect->timer >= 4) {
            effect->phase = 0;
            effect->timer = 0;
            effect->previousPosition.x = (s16) (u16) effect->position.x;
            effect->previousPosition.y = (s16) (u16) effect->position.y;
            effect->previousPosition.z = (s16) (u16) effect->position.z;
            if (command->pending != 0) {
                command->pending = 0;
                effect->phase = 3;
                endpointX = (u16) effect->endpoint.x;
                effect->target.x = (s16) (u16) command->target.x;
                effect->target.y = (s16) (u16) command->target.y;
                effect->target.z = (s16) (u16) command->target.z;
                temp_v1 = (u16) effect->endpoint.z;
                effect->commandedEndpoint.x = (s16) (u16) command->position.x;
                effect->commandedEndpoint.y = (s16) (u16) command->position.y;
                effect->commandedEndpoint.z = (s16) (u16) command->position.z;
                asm volatile("" : : "m"(effect->commandedEndpoint));
                var_a1_2 = command->duration;
                effect->previousEndpoint.x = (s16) endpointX;
                effect->previousEndpoint.z = (s16) temp_v1;
                effect->previousEndpoint.y = (s16) (u16) effect->endpoint.y;
                effect->duration = (s16) var_a1_2;
            }
        }
        break;
    case 3:                                         /* switch 1 */
        temp_v0_7 = (u16) effect->timer + 1;
        effect->timer = temp_v0_7;
        if (temp_v0_7 == 1) {
            func_8006DCE4(0x5FF, func_800D3FD8(), effect->position.x, effect->position.y, (s32) effect->position.z);
        }
        temp_s3 = func_80077CF4((s32) (effect->timer << 0xA) / (s16) effect->duration);
        temp_s0_3 = 0x1000 - temp_s3;
        func_800783E4(&effect->previousPosition, &effect->target, temp_s0_3, temp_s3, &effect->position);
        func_800783E4(&effect->previousEndpoint, &effect->commandedEndpoint, temp_s0_3, temp_s3, &effect->endpoint);
        if (effect->timer >= effect->duration) {
            effect->phase = 4;
            effect->timer = 0;
            effect->previousPosition.x = (s16) (u16) effect->position.x;
            effect->previousPosition.y = (s16) (u16) effect->position.y;
            effect->previousPosition.z = (s16) (u16) effect->position.z;
        }
        break;
    case 4:                                         /* switch 1 */
        temp_v0_9 = (u16) effect->timer + 1;
        effect->timer = temp_v0_9;
        if (temp_v0_9 == 1) {
            temp_v0_10 = func_800CE610(D_800F33E0->pool);
            if (temp_v0_10 != NULL) {
                temp_v0_10->velocity.x = (s16) (u16) effect->rotation.x;
                temp_v0_10->velocity.y = (s16) (u16) effect->rotation.y;
                temp_v0_10->velocity.z = (s16) (u16) effect->rotation.z;
                func_800CFB7C(&effect->rotation, 0x8C, &temp_v0_10->position);
                temp_v0_10->position.x = (u16) temp_v0_10->position.x + (u16) effect->position.x;
                temp_v0_10->position.y = (u16) temp_v0_10->position.y + (u16) effect->position.y;
                particleZ = (u16) temp_v0_10->position.z;
                effectZ = (u16) effect->position.z;
                temp_v0_10->kind = 0;
                temp_v0_10->timer = 0;
                temp_v0_10->position.z = particleZ + effectZ;
            }
            func_8006DCE4(0x600, 0x80, effect->position.x, effect->position.y, (s32) effect->position.z);
        }
        if (effect->timer >= 4) {
            effect->phase = 5;
            effect->timer = 0;
            temp_v0_10 = func_800CE610(D_800F33E0->pool);
            if (temp_v0_10 != NULL) {
                temp_v0_10->position.x = (s16) (u16) effect->endpoint.x;
                temp_v0_10->position.y = (s16) (u16) effect->endpoint.y;
                temp_v0_10->position.z = (s16) (u16) effect->endpoint.z;
                temp_v0_10->velocity.x = (s16) (u16) effect->rotation.x;
                temp_v0_10->velocity.y = (s16) (u16) effect->rotation.y;
                temp_v0_14 = effect->rotation.z;
                temp_v0_10->kind = 1;
                temp_v0_10->timer = 0;
                temp_v0_10->velocity.z = temp_v0_14;
            }
            for (particleIndex = 0; particleIndex < 12; ++particleIndex) {
            temp_v0_12 = func_800CE610(D_800F33E0->pool);
            if (temp_v0_12 != NULL) {
                temp_v0_12->position.x = (s16) (u16) effect->endpoint.x;
                temp_v0_12->position.y = (s16) (u16) effect->endpoint.y;
                temp_v0_12->position.z = (s16) (u16) effect->endpoint.z;
                temp_v0_12->velocity.x = (func_80071A54() % 70) - 0x23;
                temp_ret = func_80071A54();
                temp_v0_12->velocity.y = ((temp_ret / 70) * 0x46) - temp_ret;
                temp_v0_13 = func_80071A54();
                temp_v0_12->kind = 2;
                temp_v0_12->timer = 0;
                temp_v0_12->velocity.z = (temp_v0_13 % 70) - 0x23;
            }
            }
        }
        break;
    case 5:                                         /* switch 1 */
        effect->timer = (u16) effect->timer + 1;
        if ((func_800C6B90(&effect->endpoint, 0x42) != 0) && (D_80190800 == 0)) {
            temp_v1_3 = D_800F32D0->pool->node;
            *(u8 *)((u8 *)temp_v1_3 + ((temp_v1_3->flags >> 17) & 0x70) + 0x1C) = 2;
            if (D_800E2368->active != 0) {
                if ((D_800F32D0->pool->node->flags & 0x3F000000) == 0x01000000) {
                    temp_v1_4 = *D_8009D254;
                    temp_v1_4->flags |= 0x4000;
                    temp_a0 = D_800F32D0->pool->node;
                    temp_a0->flags = (temp_a0->flags & 0xC0FFFFFF) | 0x2F000000;
                    temp_a0_2 = D_800F32D0->pool->node;
                    temp_a0_2->flags |= 0x80000000;
                }
            }
            D_80190800 = 0x74;
        }
        if (effect->timer >= 8) {
            effect->phase = 0;
            effect->timer = 0;
        }
        break;
    case 6:                                         /* switch 1 */
        temp_v0_14 = (u16) effect->timer + 1;
        effect->timer = temp_v0_14;
        effect->alpha = 0x60 - (temp_v0_14 * 3);
        if (effect->timer >= 0x20) {
            return 1;
        }
        break;
    default: break;
    }
        if (effect->phase != 6) {
            effect->alpha = (func_80077CF4(D_800E27EC << 5) / 128) + 0x80;
        }
        temp_s0_2 = &effect->rotation;
        func_800CFAA8(&effect->position, &effect->endpoint, temp_s0_2);
        func_800CFB7C(temp_s0_2, 0x8C, &effect->trailHead);
        headOffsetX = (u16)effect->trailHead.x;
        headPositionX = (u16)effect->position.x;
        headPositionY = (u16)effect->position.y;
        headPositionZ = (u16)effect->position.z;
        effect->trailHead.x = headOffsetX + headPositionX;
        effect->trailHead.y = (u16) effect->trailHead.y + headPositionY;
        effect->trailHead.z = (u16) effect->trailHead.z + headPositionZ;
        func_800CFB7C(temp_s0_2, -0x8C, &effect->trailTail);
        effect->trailTail.x = (u16) effect->trailTail.x + (u16) effect->position.x;
        effect->trailTail.y = (u16) effect->trailTail.y + (u16) effect->position.y;
        effect->trailTail.z = (u16) effect->trailTail.z + (u16) effect->position.z;
        if ((D_800F32D0->pool->node->progress <= 0x7A11F) && (effect->phase != 6)) {
            effect->phase = 6;
            effect->timer = 0;
        }
        if (D_80190800 > 0) {
            D_80190800 -= 1;
            return 0;
        }
        D_80190800 = 0;
        return 0;
    }
render: {
        register SceneE20Matrix **matrixAddress asm("$5") = &D_800BCFA4;
        {
            u32 *matrixWords;
            register u32 a asm("$12"); register u32 b asm("$13"); register u32 c asm("$14");
            asm volatile("" : "=r"(matrixAddress) : "0"(matrixAddress) : "memory");
            asm volatile("" : "=r"(matrixWords) : "0"((u32 *)*matrixAddress) : "$2", "$3", "$4", "$7");
            asm volatile("" : : "r"(matrixAddress));
            a=matrixWords[0]; b=matrixWords[1];
            gte_ctc2_0(a); gte_ctc2_1(b);
            a=matrixWords[2]; b=matrixWords[3]; c=matrixWords[4];
            gte_ctc2_2(a); gte_ctc2_3(b); gte_ctc2_4(c);
            a=matrixWords[5]; b=matrixWords[6];
            gte_ctc2_5(a); c=matrixWords[7];
            gte_ctc2_6(b); gte_ctc2_7(c);
        }
        textureIndex = D_800E11EA;
        D_800F3368 = 0x20;
        D_800F336A = 2;
        D_800F3376 = 0x20;
        D_800F3378 = 0x20;
        texturePage = D_800E2850[textureIndex];
        asm volatile("" : : "r"(texturePage));
        D_800F336C = 3;
        D_800F336E = 0;
        D_800F3372 = 0;
        D_800F3374 = 4;
        D_800F3370 = texturePage;
        {
            u32 *matrixWords;
            register u32 a asm("$12"); register u32 b asm("$13"); register u32 c asm("$14");
            asm volatile("" : "=r"(matrixAddress) : "0"(matrixAddress) : "memory");
            asm volatile("" : "=r"(matrixWords) : "0"((u32 *)*matrixAddress) : "$2", "$3", "$4", "$7");
            asm volatile("" : : "r"(matrixAddress));
            a=matrixWords[0]; b=matrixWords[1];
            gte_ctc2_0(a); gte_ctc2_1(b);
            a=matrixWords[2]; b=matrixWords[3]; c=matrixWords[4];
            gte_ctc2_2(a); gte_ctc2_3(b); gte_ctc2_4(c);
            a=matrixWords[5]; b=matrixWords[6];
            gte_ctc2_5(a); c=matrixWords[7];
            gte_ctc2_6(b); gte_ctc2_7(c);
        }
        flashColor = oddColor;
        if (!(D_800E27EC & 1)) flashColor = evenColor;
        temp_v1 = effect->phase;
        switch (temp_v1) {                          /* switch 2; irregular */
        case 4:                                     /* switch 2 */
            temp_s3 = effect->timer << 0xA;
            func_800783E4(&effect->trailHead, &effect->endpoint, 0x1000 - temp_s3, temp_s3, &point);
            var_a1 = &point;
block_63:
            func_800D2B58(&effect->trailHead, var_a1, flashColor, flashColor, 128, 128, 1);
            break;
        case 5:                                     /* switch 2 */
            var_s0 = func_80077DC4(effect->timer << 7) / 32;
            func_800D2B58(&effect->trailHead, &effect->endpoint, flashColor, flashColor,
                         var_s0 / 2, var_s0, 1);
            break;
        }
        temp_v0_2 = effect->alpha;
        if (temp_v0_2 != 0) {
            temp_s3 = 1;
            if (temp_v0_2 >= 0x60) {
                temp_s3 = 0xFF;
            }
            pageArg0 = 0; pageArg1 = temp_s3; pageArg2 = 0;
            asm volatile("" : : "r"(pageArg0), "r"(pageArg1), "r"(pageArg2));
            pageIndex = &D_800E11FA;
            asm volatile("" : "=r"(pageIndex) : "0"(pageIndex));
            texturePage = firstPageTable[*pageIndex];
            asm volatile("" : : "r"(texturePage));
            D_800F336C = 3;
            D_800F336E = 1;
            D_800F3370 = texturePage;
            temp_s0 = (D_800E2850[*pageIndex] | func_80077A64(pageArg0, pageArg1, pageArg2, 0)) & 0xFFFF;
            modelScale = 0x400;
            temp_a1 = D_800E1204[D_800F336C];
            if ((D_800F336C == 4) && (D_800F3428 != 0)) {
                var_a1_2 = temp_a1 + 6;
            } else {
                var_a1_2 = temp_a1 + 2;
            }
            func_800C6EC0(temp_s0, func_80077AA4(0, var_a1_2) & 0xFFFF);
            if (temp_s3 != 0xFF) func_800C6ED8(1);
            else func_800C6ED8(0);
            drawAngle = D_800E27EC * 0x60;
            temp_v0_3 = (u16) effect->rotation.x;
            drawRotation.x = temp_v0_3;
            drawRotation.y = (u16) effect->rotation.y;
            drawRotation.z = (u16) effect->rotation.z;
            drawRotation.x = temp_v0_3 + 0x800;
            var_v0 = func_80077CF4(drawAngle);
            if (var_v0 < 0) {
                var_v0 += 7;
            }
            drawRotation.z = (var_v0 >> 3) + 0x800;
            func_80079754(&effect->rotation, &matrix);
            matrix.t[0] = (s32) effect->position.x;
            matrix.t[1] = (s32) effect->position.y;
            translationZ = effect->position.z;
            scale[0] = modelScale;
            scale[1] = modelScale;
            scale[2] = modelScale;
            matrix.t[2] = translationZ;
            func_80078CC4(&matrix, &scale[0]);
            func_800C6EF8(D_8019085C);
            func_800C6FA0(D_8019085C, (u16) effect->alpha);
            func_800C71E4(D_8019085C, &matrix);
            func_800C6F4C(D_8019085C);
            func_800D1384(&effect->trailHead, &effect->trailTail, 6, &beamColor, &edgeColor, (s32) effect->alpha, effect->history, 1);
            D_800F3374 = 0x33;
            asm volatile("" : : "m"(D_800F3374));
            var_s0 = effect->alpha;
            if (D_800E27EC & 1) {
                temp_v0_4 = var_s0 * 3;
                var_s0 = temp_v0_4 >> 2;
                if (temp_v0_4 < 0) {
                    var_s0 = (temp_v0_4 + 3) >> 2;
                }
            }
            paletteX = 0;
            paletteY = D_800E120A;
            asm volatile("" : : "r"(paletteX), "r"(paletteY));
            texturePage = D_800E2850[D_800E11FA];
            asm volatile("" : : "r"(texturePage));
            D_800F336C = 3;
            D_800F336E = 1;
            D_800F3370 = texturePage;
            func_800CEE20(&effect->position, NULL, 0x2000, 0x2000, 6, func_80077AA4(paletteX, paletteY + 5) & 0xFFFF, 3, (s32) var_s0, &beamColor);
        }
        D_800F3374 = 0xB;
        return 0;
    }
}
