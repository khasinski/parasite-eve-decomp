/* MASPSX_FLAGS: --expand-div */
#define func_80191D14 func_80191D14_header_decl
#define func_8019209C func_8019209C_header_decl
#include "../room_lib/room_lib.h"
#undef func_8019209C
#undef func_80191D14

extern int func_8019209C(RoomEnt *obj);

#define ROOMLIB_ADVANCE_ARC_Y_NAME func_80191D14
#define ROOMLIB_ADVANCE_ARC_Y_RESET func_8019209C
#include "../room_lib/RoomLib_AdvanceArcToTargetY.inc"
