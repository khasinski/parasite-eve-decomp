#include "pe1/scene_player_orb.h"

/* After the start delay, slide the orb's ribbon from the anchor toward its
 * target in sixteenth steps, burst it at step 12, and scatter the trail
 * sprites around the ribbon head. */
void func_80190B98(void *object, ScenePlayerOrbTimer *timer, ScenePlayerOrb *orb)
{
    GteShortVector step;
    ScenePlayerOrbAnchor *anchor;
    GteShortVector *spawn;
    u32 i;

    anchor = func_800C2B50();
    if (orb->delay != 0) {
        if (--orb->delay == 0) {
            orb->active = 1;
            func_800C2B90(object, 1, D_80198754, D_80198718);
        }
    }
    if (orb->active != 0) {
        orb->fade++;
        if (++orb->active == 12) {
            spawn = func_800C2B90(object, 3, D_80198754, D_80198718);
            if (spawn != 0) {
                spawn->x = orb->position.x;
                spawn->y = orb->position.y;
                spawn->z = orb->position.z;
                if (func_800C6B90(spawn, 200) != 0) {
                    anchor->orbHit = 1;
                }
            }
            timer->state = 2;
        }
    }
    step.x = (orb->position.x - orb->anchor.x) >> 4;
    step.y = (orb->position.y - orb->anchor.y) >> 4;
    step.z = (orb->position.z - orb->anchor.z) >> 4;
    orb->trailA[2].x = orb->anchor.x + step.x * (orb->active - 2);
    orb->trailA[2].y = orb->anchor.y + step.y * (orb->active - 2);
    orb->trailA[2].z = orb->anchor.z + step.z * (orb->active - 2);
    orb->trailA[0].x = orb->trailA[2].x + step.x * (orb->active - 1);
    orb->trailA[0].y = orb->trailA[2].y + step.y * (orb->active - 1);
    orb->trailA[0].z = orb->trailA[2].z + step.z * (orb->active - 1);
    orb->trailA[1].x = (orb->trailA[0].x + orb->trailA[2].x) >> 1;
    orb->trailA[1].y = (orb->trailA[0].y + orb->trailA[2].y) >> 1;
    orb->trailA[1].z = (orb->trailA[0].z + orb->trailA[2].z) >> 1;
    orb->trailB[0].x = orb->trailA[0].x;
    orb->trailB[0].y = D_800942EC.value;
    orb->trailB[0].z = orb->trailA[0].z;
    orb->trailB[1].x = orb->trailA[1].x;
    orb->trailB[1].y = D_800942EC.value;
    orb->trailB[1].z = orb->trailA[1].z;
    orb->trailB[2].x = orb->trailA[2].x;
    orb->trailB[2].y = D_800942EC.value;
    orb->trailB[2].z = orb->trailA[2].z;
    for (i = 0; i < 8; i++) {
        orb->points[i].x = orb->trailA[2].x - (step.x << 2) + func_80071A54() % 100 - 50;
        orb->points[i].y = orb->trailA[2].y;
        orb->points[i].z = orb->trailA[2].z - (step.z << 2) + func_80071A54() % 100 - 50;
    }
}
