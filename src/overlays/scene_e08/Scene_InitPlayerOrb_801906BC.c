#include "pe1/scene_player_orb.h"

/* Spawn the orb 150 units around the player, rotate that offset around the
 * scene anchor by the anchor's heading, and set the trail colours. */
void func_801906BC(void *object, void *timer, ScenePlayerOrb *orb)
{
    GteShortVector offset;
    GteShortVector rotated;
    GteRotation rotation;
    GteMatrix matrix;
    ScenePlayerOrbAnchor *anchor;

    anchor = func_800C2B50();
    rotation = D_8018EFFC;
    orb->count = 3;
    orb->anchor.x = anchor->x;
    orb->anchor.y = anchor->y;
    orb->anchor.z = anchor->z;
    orb->start.x = anchor->x;
    orb->start.y = anchor->y;
    orb->start.z = anchor->z;
    orb->position.x = g_PlayerEntity->pos_x >> 16;
    orb->position.y = g_PlayerEntity->pos_y >> 16;
    orb->position.z = g_PlayerEntity->pos_z >> 16;
    orb->position.x += func_80071A54() % 300 - 150;
    orb->position.z += func_80071A54() % 300 - 150;
    rotation.y = *func_800C2B10(2);
    func_800794C4(&rotation, &matrix);
    offset.x = orb->position.x - orb->anchor.x;
    offset.y = orb->position.y - orb->anchor.y;
    offset.z = orb->position.z - orb->anchor.z;
    func_80078C34(&matrix, &offset, &rotated);
    orb->position.x = orb->anchor.x + rotated.x;
    orb->position.y = orb->anchor.y + rotated.y;
    orb->position.z = orb->anchor.z + rotated.z;
    orb->head.r = 0x80;
    orb->head.g = 0x80;
    orb->head.b = 0xF0;
    orb->tail.r = 0x20;
    orb->tail.g = 0x20;
    orb->tail.b = 0xF0;
    orb->alpha = 0xFF;
    orb->active = 0;
    orb->fade = 0;
    orb->delay = *func_800C2B10(1);
    if (D_800B0E64.channel != 0) {
        func_8006DF50(D_800B0E64.channel, 0x606, 0, 0x80, 0x7F);
    }
}
