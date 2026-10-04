#include "pe1/scene_e18_effects.h"

/* Spinning shell controller: loads the shell model, then spins it around
 * the room model while it widens and rises; after frame 8 the spin and the
 * widening slow down, it brightens until frame 16 and fades after frame
 * 18. It finishes (setting the model's stage byte to 4) once the model's
 * seventh animation reaches its last frame. */
int func_801924E8(int mode, SceneE18SpinShell *shell) {
    GteMatrix matrix;
    GteVector scaleArg;
    GteVector scale;
    SceneE18Instance *instance = D_800F32D0->instance;
    int c;
    int s;

    switch (mode) {
    case 0:
        D_801941C0 = func_8006E498(D_800B0E64, 0xC54C7704);
        func_800C6D5C(D_801941C0, 0, 0);
        shell->width = 0x400;
        shell->heightGrowth = 0x40;
        shell->height = 0;
        shell->widthGrowth = 0;
        shell->angle = 0;
        shell->turn = -0x200;
        shell->brightness = 0;
        break;
    case 1:
        if (*instance->owner->stage == 1)
            *instance->owner->stage = 2;
        if (instance->animationId == 7
            && instance->animation.part.frame >= instance->frameLimit - 1U) {
            *instance->owner->stage = 4;
            return 1;
        }
        shell->angle += shell->turn;
        if (D_800E27EC >= 9)
            shell->turn -= 8;
        shell->width += shell->widthGrowth;
        if (shell->width < 0x40)
            shell->width = 0x40;
        if (D_800E27EC >= 9)
            shell->widthGrowth -= 2;
        shell->height += shell->heightGrowth;
        shell->heightGrowth += 6;
        if (D_800E27EC < 0x10) {
            shell->brightness += 8;
            break;
        }
        if (D_800E27EC < 0x13)
            return 0;
        if (shell->brightness != 0) {
            shell->brightness -= 5;
            if (shell->brightness < 0)
                shell->brightness = 0;
        }
        return 0;
    case 2:
        if (shell->brightness == 0)
            return 0;
        c = func_80077DC4(shell->angle);
        s = func_80077CF4(shell->angle);
        matrix.m[0][2] = s;
        matrix.m[2][0] = -s;
        matrix.m[0][0] = c;
        matrix.m[2][2] = c;
        matrix.t[2] = 0;
        matrix.t[1] = 0;
        matrix.t[0] = 0;
        matrix.m[2][1] = 0;
        matrix.m[1][2] = 0;
        matrix.m[1][0] = 0;
        matrix.m[0][1] = 0;
        matrix.m[1][1] = 0x1000;
        func_80071A44(&scale, 0, 0x10);
        scale.x = shell->width;
        scale.y = shell->height;
        scale.z = shell->width;
        scaleArg = scale;
        func_80078CC4(&matrix, &scaleArg);
        matrix.t[0] = instance->transform.t[0];
        matrix.t[1] = instance->transform.t[1];
        matrix.t[2] = instance->transform.t[2];
        {
            int kind;
            int palette;
            int page = (D_800E2850[D_800E11FA] | func_80077A64(0, 1, 0, 0)) & 0xFFFF;
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            func_800C6EC0(page, func_80077AA4(0, (kind == 4 && D_800F3428 != 0) ? palette + 8
                                                                         : palette + 4));
        }
        func_800C6ED8(1);
        func_800C6EF8(D_801941C0);
        func_800C6FA0(D_801941C0, shell->brightness);
        func_800C71E4(D_801941C0, &matrix);
        func_800C6F4C(D_801941C0);
        break;
    }
    return 0;
}
