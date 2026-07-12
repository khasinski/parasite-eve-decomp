#include "../room_lib/room_lib.h"

extern char D_80192510[];
extern char D_80192540[];
extern char D_80192558[];
extern int RoomLib_CloseTarget_80190674(RoomEnt *o);

ROOMLIB_REGISTER_PAIR_OR_CLOSE(func_801905E8, D_80192510, D_80192558, D_80192540, RoomLib_CloseTarget_80190674)
