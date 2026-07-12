#include "../room_lib/room_lib.h"

extern char D_801941CC[];
extern char D_801941F4[];
extern char D_80194208[];
extern int RoomLib_CloseTarget_8018FFBC(RoomEnt *o);

ROOMLIB_REGISTER_PAIR_STATUS3_OR_CLOSE(func_8018FF30, D_801941CC, D_80194208, D_801941F4, RoomLib_CloseTarget_8018FFBC)
