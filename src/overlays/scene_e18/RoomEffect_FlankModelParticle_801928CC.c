#include "pe1/scene_e18_effects.h"

/* Flank model particle: swings around the room model while it widens and
 * fades; the leading particle releases the actor once the player comes
 * within a quarter of its width. Each particle that fades out counts
 * D_801941C8 down. */
int func_801928CC(int mode, SceneE18FlankParticle *particle) {
    GteMatrix matrix;
    GteVector scaleArg;
    GteVector scale;
    int c;
    int s;

    if (mode == 1) {
        particle->angle += particle->turn;
        particle->width += particle->widthGrowth;
        particle->height += particle->heightGrowth;
        particle->widthGrowth += 0x30;
        particle->heightGrowth += 0x20;
        if (particle->leader) {
            int distance;
            int dz;
            int range;
            distance = RoomMain_ActorPtr->transform.t[0] - particle->position[0];
            dz = RoomMain_ActorPtr->transform.t[2] - particle->position[2];
            distance = func_8005186C(distance * distance + dz * dz);
            range = particle->width / 4;
            if (distance < range) {
                RoomMain_ActorPtr->owner->status |= 0x4000;
                D_800F32D0->instance->owner->flags |= 0x80000000;
                particle->leader = 0;
            }
        }
        particle->brightness -= 10;
        if (particle->brightness < 10) {
            D_801941C8--;
            return 1;
        }
    } else if (mode == 2) {
        c = func_80077DC4(particle->angle);
        s = func_80077CF4(particle->angle);
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
        scale.x = particle->width;
        scale.y = particle->height;
        scale.z = particle->width;
        scaleArg = scale;
        func_80078CC4(&matrix, &scaleArg);
        matrix.t[0] = particle->position[0];
        matrix.t[1] = particle->position[1];
        matrix.t[2] = particle->position[2];
        func_800C6EF8(D_801941C4);
        func_800C6FA0(D_801941C4, particle->brightness > 0x80 ? 0x80 : particle->brightness);
        func_800C71E4(D_801941C4, &matrix);
        func_800C6F4C(D_801941C4);
    }
    return 0;
}
