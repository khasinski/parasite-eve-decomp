#include "../room_lib/room_lib.h"

extern char D_80194334[];
extern char D_8019434C[];
extern char D_80194358[];
extern int RoomLib_CloseTarget_8018F314(RoomEnt *o);

ROOMLIB_REGISTER_PAIR_OR_CLOSE(func_8018F288, D_80194334, D_80194358, D_8019434C, RoomLib_CloseTarget_8018F314)
