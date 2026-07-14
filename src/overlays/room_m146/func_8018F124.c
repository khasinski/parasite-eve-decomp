#include "../room_lib/room_lib.h"

extern char D_80192448[];
extern char D_80192480[];
extern char D_8019249C[];
extern int func_8018F1B0(RoomEnt *o);

ROOMLIB_REGISTER_PAIR_STATUS3_OR_CLOSE(func_8018F124, D_80192448, D_8019249C, D_80192480, func_8018F1B0)
