#include "../room_lib/room_lib.h"

extern char D_801953A4[];
extern char D_801953D4[];
extern char D_801953EC[];
extern int RoomLib_CloseTarget_80190344(RoomEnt *o);

ROOMLIB_REGISTER_PAIR_OR_CLOSE(func_801902B8, D_801953A4, D_801953EC, D_801953D4, RoomLib_CloseTarget_80190344)
