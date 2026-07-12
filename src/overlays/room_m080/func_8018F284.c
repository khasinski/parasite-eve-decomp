#include "../room_lib/room_lib.h"

extern char D_80193F20[];
extern char D_80193F50[];
extern char D_80193F68[];
extern int RoomLib_CloseTarget_8018F348(RoomEnt *o);

ROOMLIB_REGISTER_PAIR_UNLESS_TARGET_AC_CLEAR(func_8018F284, D_80193F20, D_80193F68, D_80193F50, RoomLib_CloseTarget_8018F348)
