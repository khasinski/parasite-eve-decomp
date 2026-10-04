#include "pe1/battle_runtime.h"
#include "pe1/battle_status.h"

/* Builds the rotating target pointer and the two connector lines that tie
 * the floating status panel to the selected combatant. */

/* Packets of the active draw slot. Every access re-reads the slot, which
 * retail does after each byte store. */
#define STATUS_POINTER_SPRITE (&D_8009E460[g_ActiveDrawSlot].sprite)
#define STATUS_POINTER_TRI (&D_8009E4D8[g_ActiveDrawSlot])

void Battle_BuildStatusPrimHeader(RenderObjectEntity *object, s8 outOfRange)
{
    GteMatrix rotation;
    GteMatrix translation;
    GteShortVector pointer[3] = { { -3, -16, 0 }, { 3, -16, 0 }, { 0, 0, 0 } };
    GteVector projected[3];
    GteShortVector angles = { 0, 0, (D_8009D250 << 6) & 0xFFF };
    GteVector offset = { object->projected_target_x, object->projected_target_y, 0 };
    s32 flag;
    s16 x;
    s16 y;

    STATUS_POINTER_SPRITE->x = object->projected_target_x - 12;
    STATUS_POINTER_SPRITE->y = object->projected_target_y - 12;
    if (outOfRange == 0) {
        STATUS_POINTER_SPRITE->color.bytes.r = 0x96;
        STATUS_POINTER_SPRITE->color.bytes.g = 0x14;
        STATUS_POINTER_SPRITE->color.bytes.b = 0x14;
        STATUS_POINTER_TRI->r = 0x96;
        STATUS_POINTER_TRI->g = 0x14;
        STATUS_POINTER_TRI->b = 0x14;
    } else {
        STATUS_POINTER_SPRITE->color.bytes.r = 0x32;
        STATUS_POINTER_SPRITE->color.bytes.g = 0xA;
        STATUS_POINTER_SPRITE->color.bytes.b = 0xA;
        STATUS_POINTER_TRI->r = 0x32;
        STATUS_POINTER_TRI->g = 0xA;
        STATUS_POINTER_TRI->b = 0xA;
    }

    RotMatrix(&angles, &rotation);
    SetRotMatrix(&rotation);
    TransMatrix(&translation, &offset);
    SetTransMatrix(&translation);
    RotTrans(&pointer[0], &projected[0], &flag);
    RotTrans(&pointer[1], &projected[1], &flag);
    RotTrans(&pointer[2], &projected[2], &flag);
    STATUS_POINTER_TRI->x0 = projected[0].x;
    STATUS_POINTER_TRI->y0 = projected[0].y;
    STATUS_POINTER_TRI->x1 = projected[1].x;
    STATUS_POINTER_TRI->y1 = projected[1].y;
    STATUS_POINTER_TRI->x2 = projected[2].x;
    STATUS_POINTER_TRI->y2 = projected[2].y;
    AddPrim((unsigned int *)D_800B0E38.ordering[g_ActiveDrawSlot] + 5,
            (unsigned int *)STATUS_POINTER_TRI);

    /* Connector lines run to the panel on whichever side has room. */
    x = object->projected_target_x;
    if (x + 134 >= 301) {
        RenderLinePacket *lines;
        s16 panelX;

        x -= 8;
        lines = D_8009E498[g_ActiveDrawSlot];
        lines[1].x0 = x;
        D_8009E498[g_ActiveDrawSlot][0].x0 = x;
        panelX = object->projected_target_x;
        lines[1].x1 = panelX - 35;
        D_8009E498[g_ActiveDrawSlot][0].x1 = panelX - 35;
        D_8009E358[g_ActiveDrawSlot * 3].x0 = panelX - 115;
    } else {
        RenderLinePacket *lines;
        s16 lineX;
        s16 panelX;

        lineX = x + 8;
        lines = D_8009E498[g_ActiveDrawSlot];
        lines[1].x0 = lineX;
        D_8009E498[g_ActiveDrawSlot][0].x0 = lineX;
        panelX = object->projected_target_x + 35;
        D_8009E358[g_ActiveDrawSlot * 3].x0 = panelX;
        lines[1].x1 = panelX;
        D_8009E498[g_ActiveDrawSlot][0].x1 = panelX;
    }
    y = object->projected_target_y;
    if (y - 82 < 0) {
        D_8009E498[g_ActiveDrawSlot][0].y0 = y + 8;
        D_8009E498[g_ActiveDrawSlot][1].y0 = object->projected_target_y + 9;
        D_8009E498[g_ActiveDrawSlot][0].y1 = D_8009E358[g_ActiveDrawSlot * 3].y0 =
            object->projected_target_y + 35;
        D_8009E498[g_ActiveDrawSlot][1].y1 = object->projected_target_y + 36;
    } else {
        D_8009E498[g_ActiveDrawSlot][0].y0 = y - 8;
        D_8009E498[g_ActiveDrawSlot][1].y0 = object->projected_target_y - 7;
        D_8009E498[g_ActiveDrawSlot][0].y1 = D_8009E358[g_ActiveDrawSlot * 3].y0 =
            object->projected_target_y - 35;
        D_8009E498[g_ActiveDrawSlot][1].y1 = object->projected_target_y - 34;
    }
    AddPrim((unsigned int *)D_800B0E38.ordering[g_ActiveDrawSlot] + 7,
            (unsigned int *)&D_8009E498[g_ActiveDrawSlot][1]);
    AddPrim((unsigned int *)D_800B0E38.ordering[g_ActiveDrawSlot] + 6,
            (unsigned int *)&D_8009E498[g_ActiveDrawSlot][0]);
    AddPrim((unsigned int *)D_800B0E38.ordering[g_ActiveDrawSlot] + 5,
            (unsigned int *)&D_8009E460[g_ActiveDrawSlot]);
}
