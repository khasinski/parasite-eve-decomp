#include "../room_lib/room_lib.h"

extern char D_80191FDC[];
extern char D_80192014[];
extern char D_80192030[];
extern int func_8018F1BC(void *);

ROOMLIB_REGISTER_PAIR_STATUS3_OR_CLOSE(func_8018F130, D_80191FDC, D_80192030, D_80192014, func_8018F1BC)
