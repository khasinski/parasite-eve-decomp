/*
 * Room module class, slot 6: the no-op the class table lists last, after
 * RoomLib_CloseTarget. The same function in every room that links it on its
 * own (pe1/room_module.h).
 */
#include "pe1/room_module.h"

int RoomLib_ClassNop6(void) {
    return 0;
}
