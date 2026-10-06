/*
 * The scene script's two single-argument entries of the spark barrage
 * rooms (room_m188, room_m390): each stores its argument in its own word of
 * the room's data and returns where it went. Linked right after the
 * barrage module (RoomEffect_SparkBarrage).
 */
#include "pe1/room_module.h"

int *RoomLib_SetArgA(int unused, int value) {
    int *arg = &g_RoomScriptArgA;

    *arg = value;
    return arg;
}

int *RoomLib_SetArgB(int unused, int value) {
    int *arg = &g_RoomScriptArgB;

    *arg = value;
    return arg;
}
