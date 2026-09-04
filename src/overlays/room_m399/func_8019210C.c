#define func_8019210C func_8019210C_header_decl
#define RoomLib_ResetAndSignal_801924AC RoomLib_ResetAndSignal_801924AC_header_decl
#include "../room_lib/room_lib.h"
#undef RoomLib_ResetAndSignal_801924AC
#undef func_8019210C

extern int RoomLib_ResetAndSignal_801924AC(RoomEnt *obj);

#define ROOMLIB_ADVANCE_ARC_NAME func_8019210C
#define ROOMLIB_ADVANCE_ARC_RESET RoomLib_ResetAndSignal_801924AC
#include "../room_lib/RoomLib_AdvanceArcToTarget.inc"
