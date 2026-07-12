#include "../room_lib/room_lib.h"

extern char D_80194138[];
extern char D_80194150[];
extern char D_8019415C[];
extern int RoomLib_CloseTarget_8018F338(RoomEnt *o);

ROOMLIB_REGISTER_PAIR_OR_CLOSE(func_8018F2AC, D_80194138, D_8019415C, D_80194150, RoomLib_CloseTarget_8018F338)
