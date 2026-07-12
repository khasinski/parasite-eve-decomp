#include "../room_lib/room_lib.h"

extern char D_80193184[];
extern char D_8019319C[];
extern char D_801931A8[];
extern int RoomLib_CloseTarget_8018F318(RoomEnt *o);

ROOMLIB_REGISTER_PAIR_OR_CLOSE(func_8018F28C, D_80193184, D_801931A8, D_8019319C, RoomLib_CloseTarget_8018F318)
