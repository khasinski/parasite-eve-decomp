#include "../room_lib/room_lib.h"

extern char D_801954BC[];
extern char D_801954F4[];
extern char D_80195510[];
extern int func_8018F358(RoomEnt *o);

ROOMLIB_REGISTER_PAIR_STATUS3_OR_CLOSE(func_8018F2CC, D_801954BC, D_80195510, D_801954F4, func_8018F358)
