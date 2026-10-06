/*
 * Scene script entry of room_m005, room_m023 and room_m123: in mode 1 it
 * sets the first script variable of the running field task, and it always
 * returns the room's record the script reads back. The record is each
 * room's own data with its own layout (m005's dialog anchor, m023's anchor
 * vector, m123's burst parameter), so it is only named here.
 */
#include "pe1/field_anim.h"

extern struct RoomScriptRecord g_RoomScriptRecord;

void *RoomLib_SetTaskVar0(int mode) {
    if (mode == 1) {
        D_800E2368->variables[0] = 1;
    }
    return &g_RoomScriptRecord;
}
