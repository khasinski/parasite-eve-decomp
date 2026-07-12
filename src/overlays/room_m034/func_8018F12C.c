#include "../room_lib/room_lib.h"

extern char D_8018FF34[];
extern char D_8018FF5C[];
extern char D_8018FF70[];
extern int RoomLib_CloseTarget_8018F1B8(RoomEnt *o);

ROOMLIB_REGISTER_PAIR_STATUS3_OR_CLOSE(func_8018F12C, D_8018FF34, D_8018FF70, D_8018FF5C, RoomLib_CloseTarget_8018F1B8)
