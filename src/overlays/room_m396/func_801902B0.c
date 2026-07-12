#include "../room_lib/room_lib.h"

extern char D_801946D0[];
extern char D_80194700[];
extern char D_80194718[];
extern int RoomLib_CloseTarget_8019033C(RoomEnt *o);

ROOMLIB_REGISTER_PAIR_OR_CLOSE(func_801902B0, D_801946D0, D_80194718, D_80194700, RoomLib_CloseTarget_8019033C)
