#include "../room_lib/room_lib.h"

extern char D_80190E30[];
extern char D_80190E60[];
extern char D_80190E78[];
extern int RoomLib_CloseTarget_8018F1A4(RoomEnt *o);

ROOMLIB_REGISTER_PAIR_OR_CLOSE(func_8018F118, D_80190E30, D_80190E78, D_80190E60, RoomLib_CloseTarget_8018F1A4)
