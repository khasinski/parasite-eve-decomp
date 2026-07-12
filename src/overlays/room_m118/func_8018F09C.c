#include "../room_lib/room_lib.h"

extern char D_80190094[];
extern char D_801900C4[];
extern char D_801900DC[];
extern int RoomLib_CloseTarget_8018F128(RoomEnt *o);

ROOMLIB_REGISTER_PAIR_OR_CLOSE(func_8018F09C, D_80190094, D_801900DC, D_801900C4, RoomLib_CloseTarget_8018F128)
