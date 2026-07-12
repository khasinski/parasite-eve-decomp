#include "../room_lib/room_lib.h"

extern char D_801941C0[];
extern char D_801941D8[];
extern char D_801941E4[];
extern int RoomLib_CloseTarget_8018F308(RoomEnt *o);

ROOMLIB_REGISTER_PAIR_OR_CLOSE(func_8018F27C, D_801941C0, D_801941E4, D_801941D8, RoomLib_CloseTarget_8018F308)
