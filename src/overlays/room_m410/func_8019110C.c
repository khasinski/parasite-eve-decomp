#define func_8019110C func_8019110C_header_decl
#define func_801914AC func_801914AC_header_decl
#include "../room_lib/room_lib.h"
#undef func_801914AC
#undef func_8019110C

extern int func_801914AC(RoomEnt *obj);

#define ROOMLIB_ADVANCE_ARC_NAME func_8019110C
#define ROOMLIB_ADVANCE_ARC_RESET func_801914AC
#include "../room_lib/RoomLib_AdvanceArcToTarget.inc"
