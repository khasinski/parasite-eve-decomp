/* MASPSX_FLAGS: --expand-div */
#define func_8019110C func_8019110C_header_decl
#define RoomLib_ResetAndSignal_801914AC RoomLib_ResetAndSignal_801914AC_header_decl
#include "../room_lib/room_lib.h"
#undef RoomLib_ResetAndSignal_801914AC
#undef func_8019110C

extern int RoomLib_ResetAndSignal_801914AC(RoomEnt *obj);

#define ROOMLIB_ADVANCE_ARC_NAME func_8019110C
#define ROOMLIB_ADVANCE_ARC_RESET RoomLib_ResetAndSignal_801914AC
#include "../room_lib/RoomLib_AdvanceArcToTarget.inc"
