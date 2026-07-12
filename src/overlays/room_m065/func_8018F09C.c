#include "../room_lib/room_lib.h"

extern char D_8018FFF8[];
extern char D_80190010[];
extern char D_8019001C[];
extern int RoomLib_CloseTarget_8018F128(RoomEnt *o);

ROOMLIB_REGISTER_PAIR_OR_CLOSE(func_8018F09C, D_8018FFF8, D_8019001C, D_80190010, RoomLib_CloseTarget_8018F128)
