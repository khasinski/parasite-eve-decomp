#include "../room_lib/room_lib.h"

extern char D_80196C7C[];
extern char D_80196CA4[];
extern char D_80196CB8[];
extern int RoomLib_CloseTarget_8018F400(RoomEnt *o);

ROOMLIB_REGISTER_PAIR_STATUS3_OR_CLOSE(func_8018F374, D_80196C7C, D_80196CB8, D_80196CA4, RoomLib_CloseTarget_8018F400)
