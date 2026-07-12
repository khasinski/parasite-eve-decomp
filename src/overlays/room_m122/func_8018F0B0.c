#include "../room_lib/room_lib.h"

extern char D_80190EDC[];
extern char D_80190F0C[];
extern char D_80190F24[];
extern int RoomLib_CloseTarget_8018F13C(RoomEnt *o);

ROOMLIB_REGISTER_PAIR_OR_CLOSE(func_8018F0B0, D_80190EDC, D_80190F24, D_80190F0C, RoomLib_CloseTarget_8018F13C)
