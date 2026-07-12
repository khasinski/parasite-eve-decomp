#include "../room_lib/room_lib.h"

extern char D_801934EC[];
extern char D_80193504[];
extern char D_80193510[];
extern int RoomLib_CloseTarget_8018F300(RoomEnt *o);

ROOMLIB_REGISTER_PAIR_OR_CLOSE(func_8018F274, D_801934EC, D_80193510, D_80193504, RoomLib_CloseTarget_8018F300)
