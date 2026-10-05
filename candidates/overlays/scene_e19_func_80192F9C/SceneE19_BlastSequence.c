/* MASPSX_FLAGS: --expand-div */
#include "scene_e19_blast.h"

#ifdef LEGACY
#define LOAD_VIEW_MATRIX() { RenderMatrixSlot *slot_ = &D_800BCFA4; gte_ldrotmatrix(slot_->value); gte_ldtransmatrix(slot_->value); }
#else
#define LOAD_VIEW_MATRIX() { \
    const u32 *matrix_ = (const u32 *)D_800BCFA4.value; \
    u32 a_, b_, c_; \
    a_ = matrix_[0]; b_ = matrix_[1]; gte_ctc2_0(a_); gte_ctc2_1(b_); \
    a_ = matrix_[2]; b_ = matrix_[3]; c_ = matrix_[4]; gte_ctc2_2(a_); gte_ctc2_3(b_); gte_ctc2_4(c_); \
    a_ = matrix_[5]; b_ = matrix_[6]; gte_ctc2_5(a_); c_ = matrix_[7]; gte_ctc2_6(b_); gte_ctc2_7(c_); }
#endif

int func_80192F9C(int mode, SceneE19Blast *blast)
{
    GteShortVector center;
    GteShortVector ringPoint;
    GteRotation flareRotation;
    GteShortVector rotation;
    RenderColor glowColor;
    RenderColor flareColor;
    GteShortVector floorGlow0;
    GteShortVector floorRotation0;
    GteShortVector floorGlow1;
    GteShortVector floorRotation1;
    GteMatrix shellMatrix1;
    GteVector shellScale1;
    GteShortVector floorGlow2;
    GteShortVector floorRotation2;
    GteMatrix shellMatrix2;
    GteVector shellScale2;
    GteMatrix shellMatrix3a;
    GteVector shellScale3a;
    GteMatrix shellMatrix3b;
    GteVector shellScale3b;
    GteMatrix shellMatrix3c;
    GteVector shellScale3c;
    GteMatrix shellMatrix3d;
    GteVector shellScale3d;
    GteMatrix shellMatrix3e;
    GteVector shellScale3e;
    GteMatrix shellMatrix4a;
    GteVector shellScale4a;
    GteMatrix shellMatrix4b;
    GteVector shellScale4b;
    GteMatrix shellMatrix4c;
    GteVector shellScale4c;
    GteMatrix shellMatrix4d;
    GteVector shellScale4d;
    SceneE19BlastSpark *spark;
    SceneE19BlastActor *actor;
    u16 *pageSelector;
    RenderEffectParameters *params;
    u16 *palettes;
    GteShortVector *target;
    int spread;
    int phase;
    int modelPhase;
    int intensity;
    int radialScale;
    int verticalScale;
    int page;
    int palette;
    int ringRadius;
    int angle;
    int i;
    int value;
    int height;
    s16 timer;

    flareRotation = D_8018F1D4;
    glowColor = D_8018F1DC;
    flareColor = D_8018F1E0;
    switch (mode) {
    case 0:
        D_8019B680 = func_8006E498(D_800B0E64.channel, 0xC54C0704);
        func_800C6D5C(D_8019B680, 0, 0);
        D_8019B684 = func_8006E498(D_800B0E64.channel, 0xC58C0704);
        func_800C6D5C(D_8019B684, 0, 0);
        D_8019B688 = func_8006E498(D_800B0E64.channel, 0xC5CC0704);
        func_800C6D5C(D_8019B688, 0, 0);
        D_8019B68C = func_8006E498(D_800B0E64.channel, 0xC60C0704);
        func_800C6D5C(D_8019B68C, 0, 0);
        D_8019B690 = func_8006E498(D_800B0E64.channel, 0xC64C0704);
        func_800C6D5C(D_8019B690, 0, 0);
        blast->state = 0;
        blast->timer = 0;
        func_800CE870((char *)D_800F32D0->pool, 1, &blast->position.x);
        blast->position.y = D_800942EC;
        func_800CE870((char *)D_800F32D0->pool, 0, &blast->target.x);
        func_800CE9D4((struct RoomFxTransformOwner *)D_800F32D0->pool, 0, &blast->origin);
        return func_800CE560(D_800F33E0->pool, 0xC, 8, func_80192E08);
    case 1:
        switch (blast->state) {
        case 0:
            timer = blast->timer + 1;
            blast->timer = timer;
            if (timer == 2 && D_800B0E64.channel != 0) {
                func_8006DF50(D_800B0E64.channel, 0x5E3, func_800D3FD8(), 0x80, 0x7F);
                if (D_800B0E64.channel != 0) {
                    func_8006DF50(D_800B0E64.channel, 0x5E4, 0x80, 0x80, 0x7F);
                }
            }
            if (blast->timer < 0x10) {
                return 0;
            }
            blast->state = 1;
            blast->timer = 0;
            break;
        case 1:
            timer = blast->timer + 1;
            blast->timer = timer;
            if (timer < 0x10) {
                return 0;
            }
            blast->state = 2;
            blast->timer = 0;
            break;
        case 2:
            timer = blast->timer + 1;
            blast->timer = timer;
            if (timer < 0x10) {
                return 0;
            }
            blast->state = 3;
            blast->timer = 0;
            break;
        case 3:
            blast->timer++;
            if (func_800C6B90(&blast->position, 0x960) != 0
                && (func_800C6B90(&blast->position, 0x4B0) != 0
                    || func_800C6B90(&blast->position, 0x6A4) == 0)
                && D_800E2368->active != 0
                && ((*D_800F32D0->pool)->flags & 0x3F000000) == 0x01000000) {
                (*D_8009D254)->flags |= 0x4000;
                actor = *D_800F32D0->pool;
                actor->flags = (actor->flags & 0xC0FFFFFF) | 0x2D000000;
                actor = *D_800F32D0->pool;
                actor->flags |= 0x80000000;
            }
            if (blast->timer < 0x11) {
                func_800D1D24(2, 0x10, blast->timer);
            }
            spread = 0x200;
            spark = func_800CE610(D_800F33E0->pool);
            if (spark != 0) {
                spark->x = blast->position.x;
                spark->y = blast->position.y;
                spark->z = blast->position.z;
                spark->y -= func_80071A54() % 800;
                spark->x += func_80071A54() % spread - 0x100;
                value = func_80071A54() % spread;
                spark->state = 0;
                spark->timer = 0;
                spark->z += value - 0x100;
            }
            D_8019B668 = 0x80;
            if (blast->timer < 0x20) {
                return 0;
            }
            blast->state = 4;
            blast->timer = 0;
            break;
        case 4:
            blast->timer++;
            spread = 0x200;
            spark = func_800CE610(D_800F33E0->pool);
            if (spark != 0) {
                spark->x = blast->position.x;
                spark->y = blast->position.y;
                spark->z = blast->position.z;
                spark->y -= func_80071A54() % 800;
                spark->x += func_80071A54() % spread - 0x100;
                value = func_80071A54() % spread;
                spark->state = 0;
                spark->timer = 0;
                spark->z += value - 0x100;
            }
            D_8019B668 = func_80077DC4((blast->timer << 10) / 24) / 32;
            if (blast->timer < 0x18) {
                return 0;
            }
            blast->state = 4;
            blast->timer = 0;
            return 1;
        }
        break;
    case 2:
        center.x = blast->position.x;
        center.y = blast->position.y;
        center.z = blast->position.z;
        LOAD_VIEW_MATRIX();
        D_800F3368.parameter0A = 0;
        D_800F3368.depth = 4;
        switch (blast->state) {
        case 0:
            intensity = func_80077CF4(blast->timer << 6) / 32;
            D_800F3368.parameter00 = 0x40;
            D_800F3368.parameter02 = 4;
            D_800F3368.extent_x = 0x40;
            D_800F3368.extent_y = 0x40;
            D_800F3368.tpage = D_800E2850[D_800E11FA];
            D_800F3368.palette = 3;
            D_800F3368.parameter06 = 1;
            if (D_800E27EC & 1) {
                intensity = intensity * 2 / 3;
            }
            floorRotation0.pad = 1;
            floorRotation0.x = 0x400;
            floorRotation0.y = 0;
            floorRotation0.z = 0;
            floorGlow0.x = center.x;
            floorGlow0.z = center.z;
            floorGlow0.y = D_800942EC;
            func_800CEE20(&floorGlow0, (GteRotation *)&floorRotation0, 0x2000, 0x2000, 0,
                          func_80077AA4(0, D_800E120A + 2), 1, intensity, 0);
            break;
        case 1:
            phase = blast->timer << 6;
            intensity = 0x80;
            D_800F3368.parameter00 = 0x40;
            D_800F3368.parameter02 = 4;
            D_800F3368.extent_x = 0x40;
            D_800F3368.extent_y = 0x40;
            D_800F3368.tpage = D_800E2850[D_800E11FA];
            D_800F3368.palette = 3;
            D_800F3368.parameter06 = 1;
            if (D_800E27EC & 1) {
                intensity = 0x55;
            }
            floorRotation1.pad = 1;
            floorRotation1.x = 0x400;
            floorRotation1.y = 0;
            floorRotation1.z = 0;
            floorGlow1.x = center.x;
            floorGlow1.z = center.z;
            floorGlow1.y = D_800942EC;
            func_800CEE20(&floorGlow1, (GteRotation *)&floorRotation1, 0x2000, 0x2000, 0,
                          func_80077AA4(0, D_800E120A + 2), 1, intensity, 0);
            intensity = func_80077CF4(phase) / 32;
            if (D_800E27EC & 1) {
                intensity = intensity * 2 / 3;
            }
            radialScale = func_80077CF4(phase);
            if (D_800E27EC & 1) {
                radialScale = radialScale * 15 / 16;
            }
            rotation.x = -0x400;
            rotation.y = 0;
            rotation.z = D_800E27EC << 5;
            D_800F3368.tpage = D_800E2850[D_800E11EA[0]];
            D_800F3368.palette = 3;
            D_800F3368.parameter06 = 0;
            page = (u16)(D_800E2850[D_800E11EA[0]] | func_80077A64(0, 1, 0, 0));
            palette = D_800E1204[D_800F336C];
            if (D_800F336C == 4 && D_800F3428 != 0) {
                palette += 4;
            }
            func_800C6EC0(page, func_80077AA4(0x20, palette));
            func_800C6ED8(1);
            func_80079754(&rotation, &shellMatrix1);
            shellScale1.x = radialScale / 2;
            shellScale1.y = radialScale / 2;
            shellMatrix1.t[0] = center.x;
            shellMatrix1.t[1] = center.y;
            shellMatrix1.t[2] = center.z;
            shellScale1.z = (blast->timer << 6) + 0x400;
            func_80078CC4(&shellMatrix1, &shellScale1);
            func_800C6EF8(D_8019B684);
            func_800C6FA0(D_8019B684, 0x40);
            func_800C71E4(D_8019B684, &shellMatrix1);
            func_800C6F4C(D_8019B684);
            LOAD_VIEW_MATRIX();
            func_800D004C(&center, 0x12C, 0x12C, 0xC, 0, 0x1000, 0x1000, &glowColor, 0, intensity, 1);
            intensity = func_80077DC4(phase) / 32;
            radialScale = func_80077CF4(phase);
            func_800D0728(&center, 0x7D0, 0xB54, 0x18, &flareRotation, radialScale, radialScale, 0,
                          &glowColor, intensity, 1);
            break;
        case 2:
            phase = blast->timer << 6;
            pageSelector = &D_800E11EA[8];
            params = &D_800F3368;
            intensity = 0x80;
            params->parameter00 = 0x40;
            D_800F336A = 4;
            D_800F3376 = 0x40;
            D_800F3378 = 0x40;
            D_800F3370 = D_800E2850[pageSelector[0]];
            D_800F336C = 3;
            D_800F336E = 1;
            if (D_800E27EC & 1) {
                intensity = 0x55;
            }
            floorRotation2.pad = 1;
            floorRotation2.x = 0x400;
            radialScale = 0x1000;
            floorRotation2.y = 0;
            floorRotation2.z = 0;
            floorGlow2.x = center.x;
            floorGlow2.z = center.z;
            floorGlow2.y = D_800942EC;
            func_800CEE20(&floorGlow2, (GteRotation *)&floorRotation2, 0x2000, 0x2000, 0,
                          func_80077AA4(0, D_800E120A + 2), 1, intensity, 0);
            palettes = D_800E1204;
            if (D_800E27EC & 1) {
                radialScale = 0xF00;
            }
            params->parameter06 = 0;
            rotation.x = -0x400;
            rotation.z = D_800E27EC << 5;
            params->tpage = D_800E2850[pageSelector[-8]];
            params->palette = 3;
            rotation.y = 0;
            page = (u16)(D_800E2850[pageSelector[-8]] | func_80077A64(0, 1, 0, 0));
            palette = palettes[params->palette];
            if (params->palette == 4 && D_800F3428 != 0) {
                palette += 4;
            }
            func_800C6EC0(page, func_80077AA4(0x20, palette));
            func_800C6ED8(1);
            func_80079754(&rotation, &shellMatrix2);
            shellScale2.x = radialScale / 2;
            shellScale2.y = radialScale / 2;
            shellMatrix2.t[0] = center.x;
            shellMatrix2.t[1] = center.y;
            shellMatrix2.t[2] = center.z;
            shellScale2.z = (blast->timer << 6) + 0x800;
            func_80078CC4(&shellMatrix2, &shellScale2);
            func_800C6EF8(D_8019B684);
            func_800C6FA0(D_8019B684, 0x40);
            func_800C71E4(D_8019B684, &shellMatrix2);
            func_800C6F4C(D_8019B684);
            LOAD_VIEW_MATRIX();
            func_800D004C(&center, 0x12C, 0x12C, 0xC, 0, 0x1000, 0x1000, &glowColor, 0, intensity, 1);
            intensity = 0x80;
            if (D_800E27EC & 1) {
                intensity = 0x64;
            }
            ringRadius = 2000;
            radialScale = func_80077DC4(phase) / 8;
            height = func_80077CF4(phase) * 2 / 3;
            rotation.x = 0x400;
            rotation.y = 0;
            rotation.z = 0;
            for (i = 0; i < 0x10; i++) {
                angle = (i << 8) + D_800E27EC * 4;
                ringPoint.x = center.x;
                ringPoint.y = center.y;
                ringPoint.z = center.z;
                ringPoint.x += func_80077DC4(angle) * ringRadius / 4096;
                value = func_80077CF4(angle) * ringRadius;
                rotation.z = angle + 0x400;
                ringPoint.z += value / 4096;
                func_800D0E88(&ringPoint, (GteRotation *)&rotation, height, radialScale,
                              &glowColor, 0, 0, (s16)intensity, 1);
            }
            intensity = func_80077DC4(phase) / 32;
            radialScale = func_80077DC4(phase);
            target = &blast->target;
            rotation.x = blast->origin.x;
            rotation.y = blast->origin.y;
            rotation.z = blast->origin.z;
            rotation.x -= 0x200;
            rotation.y += 0x400;
            func_800D0728(target, 0x7D0, 0xA8C, 0x20, (GteRotation *)&rotation, radialScale, radialScale,
                          &flareColor, 0, intensity, 1);
            rotation.x += 0x400;
            func_800D0728(target, 0x7D0, 0xA8C, 0x20, (GteRotation *)&rotation, radialScale, radialScale,
                          &flareColor, 0, intensity, 1);
            break;
        case 3:
            timer = blast->timer;
            phase = timer << 5;
            if (timer == 0) {
                func_800D1AE0(&glowColor, 0x46, 2, 8);
            } else if (timer < 9) {
                func_800D1AE0(&glowColor, 0x80 - timer * 0x10, 1, 8);
            }
            radialScale = func_80077DC4(phase) / 2 + 0x400;
            verticalScale = func_80077CF4(phase) / 6 + 0x555;
            intensity = 0x80;
            if (D_800E27EC & 1) {
                intensity = 0x78;
            }
            rotation.x = 0;
            rotation.y = D_800E27EC << 5;
            rotation.z = 0;
            D_800F3368.tpage = D_800E2850[D_800E11EA[0]];
            D_800F3368.palette = 3;
            D_800F3368.parameter06 = 0;
            page = (u16)(D_800E2850[D_800E11EA[0]] | func_80077A64(0, 1, 0, 0));
            palette = D_800E1204[D_800F336C];
            if (D_800F336C == 4 && D_800F3428 != 0) {
                palette += 4;
            }
            func_800C6EC0(page, func_80077AA4(0x20, palette));
            func_800C6ED8(1);
            func_80079754(&rotation, &shellMatrix3a);
            shellScale3a.x = radialScale;
            shellScale3a.y = verticalScale;
            shellScale3a.z = radialScale;
            shellMatrix3a.t[0] = center.x;
            shellMatrix3a.t[1] = center.y;
            shellMatrix3a.t[2] = center.z;
            func_80078CC4(&shellMatrix3a, &shellScale3a);
            func_800C6EF8(D_8019B680);
            func_800C6FA0(D_8019B680, (u16)intensity);
            func_800C71E4(D_8019B680, &shellMatrix3a);
            func_800C6F4C(D_8019B680);
            radialScale = func_80077CF4(phase) / 3 + 0x400;
            verticalScale = func_80077DC4(phase) / 2;
            intensity = func_80077DC4(phase) / 32;
            if (D_800E27EC & 1) {
                intensity = intensity * 15 / 16;
            }
            rotation.x = 0;
            rotation.y = -D_800E27EC * 0x30;
            rotation.z = 0;
            D_800F3368.tpage = D_800E2850[D_800E11EA[0]];
            D_800F3368.palette = 3;
            D_800F3368.parameter06 = 0;
            page = (u16)(D_800E2850[D_800E11EA[0]] | func_80077A64(0, 1, 0, 0));
            palette = D_800E1204[D_800F336C];
            if (D_800F336C == 4 && D_800F3428 != 0) {
                palette += 4;
            }
            func_800C6EC0(page, func_80077AA4(0x20, palette));
            func_800C6ED8(1);
            func_80079754(&rotation, &shellMatrix3b);
            shellScale3b.x = radialScale;
            shellScale3b.y = verticalScale;
            shellScale3b.z = radialScale;
            shellMatrix3b.t[0] = center.x;
            shellMatrix3b.t[1] = center.y;
            shellMatrix3b.t[2] = center.z;
            func_80078CC4(&shellMatrix3b, &shellScale3b);
            func_800C6EF8(D_8019B688);
            func_800C6FA0(D_8019B688, (u16)(intensity / 2));
            func_800C71E4(D_8019B688, &shellMatrix3b);
            func_800C6F4C(D_8019B688);
            modelPhase = (blast->timer << 10) / 56;
            radialScale = func_80077CF4(modelPhase) / 4 + 0xC00;
            verticalScale = func_80077DC4(modelPhase) / 4 + 0x400;
            intensity = func_80077DC4(modelPhase) / 32;
            if (D_800E27EC & 1) {
                intensity = intensity * 15 / 16;
            }
            rotation.x = 0;
            rotation.y = -D_800E27EC << 5;
            rotation.z = 0;
            D_800F3368.tpage = D_800E2850[D_800E11EA[0]];
            D_800F3368.palette = 3;
            D_800F3368.parameter06 = 0;
            page = (u16)(D_800E2850[D_800E11EA[0]] | func_80077A64(0, 1, 0, 0));
            palette = D_800E1204[D_800F336C];
            if (D_800F336C == 4 && D_800F3428 != 0) {
                palette += 4;
            }
            func_800C6EC0(page, func_80077AA4(0x20, palette));
            func_800C6ED8(1);
            height = center.y;
            center.y = height - blast->timer * 0x18;
            func_80079754(&rotation, &shellMatrix3c);
            shellScale3c.x = radialScale;
            shellScale3c.y = verticalScale;
            shellScale3c.z = radialScale;
            shellMatrix3c.t[0] = center.x;
            shellMatrix3c.t[1] = center.y;
            shellMatrix3c.t[2] = center.z;
            func_80078CC4(&shellMatrix3c, &shellScale3c);
            func_800C6EF8(D_8019B68C);
            func_800C6FA0(D_8019B68C, (u16)(intensity / 2));
            func_800C71E4(D_8019B68C, &shellMatrix3c);
            func_800C6F4C(D_8019B68C);
            center.y = height;
            LOAD_VIEW_MATRIX();
            func_800D004C(&center, 0x9C4, 0x9C4, 0xC, 0, 0x1000, 0x1000, &glowColor, 0, intensity, 1);
            radialScale = func_80077CF4(phase) / 4 + 0xC00;
            intensity = func_80077DC4(phase) / 32;
            func_800D0728(&center, 0x76C, 0xA28, 0x18, &flareRotation, radialScale, radialScale,
                          &flareColor, 0, intensity, 1);
            center.y -= 0x400;
            radialScale = func_80077CF4(phase) * 3 / 2 + 0x1000;
            func_800D0728(&center, 0x3E8, 0x5DC, 0x18, &flareRotation, radialScale, radialScale,
                          &glowColor, 0, intensity, 1);
            center.y += 0x400;
            radialScale = func_80077CF4(modelPhase) / 4 + 0x1200;
            verticalScale = func_80077DC4(modelPhase);
            intensity = func_80077DC4(modelPhase) / 32;
            if (D_800E27EC & 1) {
                intensity = intensity * 15 / 16;
            }
            rotation.x = 0;
            rotation.y = (D_800E27EC << 5) + 0x400;
            rotation.z = 0;
            D_800F3368.tpage = D_800E2850[D_800E11EA[0]];
            D_800F3368.palette = 3;
            D_800F3368.parameter06 = 0;
            page = (u16)(D_800E2850[D_800E11EA[0]] | func_80077A64(0, 1, 0, 0));
            palette = D_800E1204[D_800F336C];
            if (D_800F336C == 4 && D_800F3428 != 0) {
                palette += 4;
            }
            func_800C6EC0(page, func_80077AA4(0x60, palette));
            func_800C6ED8(1);
            func_80079754(&rotation, &shellMatrix3d);
            shellScale3d.x = radialScale;
            shellScale3d.y = verticalScale;
            shellScale3d.z = radialScale;
            shellMatrix3d.t[0] = center.x;
            shellMatrix3d.t[1] = center.y;
            shellMatrix3d.t[2] = center.z;
            func_80078CC4(&shellMatrix3d, &shellScale3d);
            func_800C6EF8(D_8019B690);
            func_800C6FA0(D_8019B690, (u16)intensity);
            func_800C71E4(D_8019B690, &shellMatrix3d);
            func_800C6F4C(D_8019B690);
            radialScale = func_80077CF4(modelPhase) / 6 + 0x1200;
            verticalScale = func_80077DC4(modelPhase) * 2;
            intensity = func_80077DC4(modelPhase) / 32;
            if (!(D_800E27EC & 1)) {
                intensity = intensity * 15 / 16;
            }
            rotation.x = 0;
            rotation.y = D_800E27EC << 5;
            rotation.z = 0;
            D_800F3368.tpage = D_800E2850[D_800E11EA[0]];
            D_800F3368.palette = 3;
            D_800F3368.parameter06 = 0;
            page = (u16)(D_800E2850[D_800E11EA[0]] | func_80077A64(0, 1, 0, 0));
            palette = D_800E1204[D_800F336C];
            if (D_800F336C == 4 && D_800F3428 != 0) {
                palette += 4;
            }
            func_800C6EC0(page, func_80077AA4(0x60, palette));
            func_800C6ED8(1);
            func_80079754(&rotation, &shellMatrix3e);
            shellScale3e.x = radialScale;
            shellScale3e.y = verticalScale;
            shellScale3e.z = radialScale;
            shellMatrix3e.t[0] = center.x;
            shellMatrix3e.t[1] = center.y;
            shellMatrix3e.t[2] = center.z;
            func_80078CC4(&shellMatrix3e, &shellScale3e);
            func_800C6EF8(D_8019B690);
            func_800C6FA0(D_8019B690, (u16)intensity);
            func_800C71E4(D_8019B690, &shellMatrix3e);
            func_800C6F4C(D_8019B690);
            break;
        case 4:
            phase = (blast->timer << 10) / 24;
            radialScale = (0x400 - phase) / 2 + 0x200;
            verticalScale = func_80077CF4(phase) / 6 + 0x6AA;
            intensity = func_80077DC4(phase) / 32;
            if (D_800E27EC & 1) {
                intensity = intensity * 15 / 16;
            }
            rotation.x = 0;
            rotation.y = D_800E27EC << 5;
            rotation.z = 0;
            D_800F3368.tpage = D_800E2850[D_800E11EA[0]];
            D_800F3368.palette = 3;
            D_800F3368.parameter06 = 0;
            page = (u16)(D_800E2850[D_800E11EA[0]] | func_80077A64(0, 1, 0, 0));
            palette = D_800E1204[D_800F336C];
            if (D_800F336C == 4 && D_800F3428 != 0) {
                palette += 4;
            }
            func_800C6EC0(page, func_80077AA4(0x20, palette));
            func_800C6ED8(1);
            func_80079754(&rotation, &shellMatrix4a);
            shellScale4a.x = radialScale;
            shellScale4a.y = verticalScale;
            shellScale4a.z = radialScale;
            shellMatrix4a.t[0] = center.x;
            shellMatrix4a.t[1] = center.y;
            shellMatrix4a.t[2] = center.z;
            func_80078CC4(&shellMatrix4a, &shellScale4a);
            func_800C6EF8(D_8019B680);
            func_800C6FA0(D_8019B680, (u16)intensity);
            func_800C71E4(D_8019B680, &shellMatrix4a);
            func_800C6F4C(D_8019B680);
            modelPhase = ((blast->timer + 0x20) << 10) / 56;
            radialScale = func_80077CF4(modelPhase) / 4 + 0xC00;
            verticalScale = func_80077DC4(modelPhase) / 4 + 0x400;
            intensity = func_80077DC4(modelPhase) / 32;
            if (D_800E27EC & 1) {
                intensity = intensity * 15 / 16;
            }
            rotation.x = 0;
            rotation.y = -D_800E27EC << 5;
            rotation.z = 0;
            D_800F3368.tpage = D_800E2850[D_800E11EA[0]];
            D_800F3368.palette = 3;
            D_800F3368.parameter06 = 0;
            page = (u16)(D_800E2850[D_800E11EA[0]] | func_80077A64(0, 1, 0, 0));
            palette = D_800E1204[D_800F336C];
            if (D_800F336C == 4 && D_800F3428 != 0) {
                palette += 4;
            }
            func_800C6EC0(page, func_80077AA4(0x20, palette));
            func_800C6ED8(1);
            height = center.y;
            center.y = height - (blast->timer + 0x20) * 0x18;
            func_80079754(&rotation, &shellMatrix4b);
            shellScale4b.x = radialScale;
            shellScale4b.y = verticalScale;
            shellScale4b.z = radialScale;
            shellMatrix4b.t[0] = center.x;
            shellMatrix4b.t[1] = center.y;
            shellMatrix4b.t[2] = center.z;
            func_80078CC4(&shellMatrix4b, &shellScale4b);
            func_800C6EF8(D_8019B68C);
            func_800C6FA0(D_8019B68C, (u16)(intensity / 2));
            func_800C71E4(D_8019B68C, &shellMatrix4b);
            func_800C6F4C(D_8019B68C);
            center.y = height;
            radialScale = func_80077CF4(modelPhase) / 4 + 0x1200;
            verticalScale = func_80077DC4(modelPhase);
            intensity = func_80077DC4(modelPhase) / 32;
            if (D_800E27EC & 1) {
                intensity = intensity * 15 / 16;
            }
            rotation.x = 0;
            rotation.y = (D_800E27EC << 5) + 0x400;
            rotation.z = 0;
            D_800F3368.tpage = D_800E2850[D_800E11EA[0]];
            D_800F3368.palette = 3;
            D_800F3368.parameter06 = 0;
            page = (u16)(D_800E2850[D_800E11EA[0]] | func_80077A64(0, 1, 0, 0));
            palette = D_800E1204[D_800F336C];
            if (D_800F336C == 4 && D_800F3428 != 0) {
                palette += 4;
            }
            func_800C6EC0(page, func_80077AA4(0x60, palette));
            func_800C6ED8(1);
            func_80079754(&rotation, &shellMatrix4c);
            shellScale4c.x = radialScale;
            shellScale4c.y = verticalScale;
            shellScale4c.z = radialScale;
            shellMatrix4c.t[0] = center.x;
            shellMatrix4c.t[1] = center.y;
            shellMatrix4c.t[2] = center.z;
            func_80078CC4(&shellMatrix4c, &shellScale4c);
            func_800C6EF8(D_8019B690);
            func_800C6FA0(D_8019B690, (u16)intensity);
            func_800C71E4(D_8019B690, &shellMatrix4c);
            func_800C6F4C(D_8019B690);
            radialScale = func_80077CF4(modelPhase) / 6 + 0x1200;
            verticalScale = func_80077DC4(modelPhase) * 2;
            intensity = func_80077DC4(modelPhase) / 32;
            if (!(D_800E27EC & 1)) {
                intensity = intensity * 15 / 16;
            }
            rotation.x = 0;
            rotation.y = D_800E27EC << 5;
            rotation.z = 0;
            D_800F3368.tpage = D_800E2850[D_800E11EA[0]];
            D_800F3368.palette = 3;
            D_800F3368.parameter06 = 0;
            page = (u16)(D_800E2850[D_800E11EA[0]] | func_80077A64(0, 1, 0, 0));
            palette = D_800E1204[D_800F336C];
            if (D_800F336C == 4 && D_800F3428 != 0) {
                palette += 4;
            }
            func_800C6EC0(page, func_80077AA4(0x60, palette));
            func_800C6ED8(1);
            func_80079754(&rotation, &shellMatrix4d);
            shellScale4d.x = radialScale;
            shellScale4d.y = verticalScale;
            shellScale4d.z = radialScale;
            shellMatrix4d.t[0] = center.x;
            shellMatrix4d.t[1] = center.y;
            shellMatrix4d.t[2] = center.z;
            func_80078CC4(&shellMatrix4d, &shellScale4d);
            func_800C6EF8(D_8019B690);
            func_800C6FA0(D_8019B690, (u16)intensity);
            func_800C71E4(D_8019B690, &shellMatrix4d);
            func_800C6F4C(D_8019B690);
            break;
        }
        D_800F3368.parameter00 = 0x10;
        D_800F3368.parameter02 = 1;
        D_800F3368.extent_x = 0x10;
        D_800F3368.extent_y = 0x10;
        D_800F3368.extent_x = 0x80;
        D_800F3368.extent_y = 0x10;
        D_800F3368.tpage = D_800E2850[D_800E11EA[0]];
        D_800F3368.palette = 3;
        D_800F3368.parameter06 = 0;
        D_800F3368.parameter0A = 0;
        break;
    }
    return 0;
}
