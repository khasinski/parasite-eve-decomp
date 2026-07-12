#include "../room_lib/room_lib.h"

extern char D_80194484[];
extern char D_801944B4[];
extern char D_801944CC[];
extern int RoomLib_CloseTarget_8018F34C(RoomEnt *o);

ROOMLIB_REGISTER_PAIR_OR_CLOSE(func_8018F2C0, D_80194484, D_801944CC, D_801944B4, RoomLib_CloseTarget_8018F34C)
