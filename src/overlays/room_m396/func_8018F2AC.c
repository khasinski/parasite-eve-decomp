#include "../room_lib/room_lib.h"

extern char D_80194640[];
extern char D_80194658[];
extern char D_80194664[];
extern int RoomLib_CloseTarget_8018F338(RoomEnt *o);

ROOMLIB_REGISTER_PAIR_OR_CLOSE(func_8018F2AC, D_80194640, D_80194664, D_80194658, RoomLib_CloseTarget_8018F338)
