#include "../room_lib/room_lib.h"

extern char D_80195314[];
extern char D_8019532C[];
extern char D_80195338[];
extern int RoomLib_CloseTarget_8018F340(RoomEnt *o);

ROOMLIB_REGISTER_PAIR_OR_CLOSE(func_8018F2B4, D_80195314, D_80195338, D_8019532C, RoomLib_CloseTarget_8018F340)
