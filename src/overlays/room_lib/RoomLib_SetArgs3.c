/*
 * The scene script's argument entry: stores its three arguments for the
 * room's effects and returns them. Thirty-five rooms and scenes link it as
 * the function after their last effect module (the orbit flare, the homing
 * flash or the sound burst); the words are the room's
 * (g_RoomScriptArgs, pe1/room_module.h).
 */
#include "pe1/room_module.h"

int *RoomLib_SetArgs3(int unused, int a, int b, int c) {
    int *args = g_RoomScriptArgs;

    *args = a;
    g_RoomScriptArgs[1] = b;
    g_RoomScriptArgs[2] = c;
    return args;
}
