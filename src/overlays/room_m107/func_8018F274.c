#include "../room_lib/room_lib.h"

extern char D_80193588[];
extern char D_801935B8[];
extern char D_801935D0[];
extern int RoomLib_CloseTarget_8018F300(RoomEnt *o);

ROOMLIB_REGISTER_PAIR_OR_CLOSE(func_8018F274, D_80193588, D_801935D0, D_801935B8, RoomLib_CloseTarget_8018F300)
