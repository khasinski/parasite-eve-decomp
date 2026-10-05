/* MASPSX_FLAGS: --expand-div */
#define func_80191158 func_80191158_header_decl
#define RoomLib_ResetAndSignal_801914F8 RoomLib_ResetAndSignal_801914F8_header_decl
#include "../room_lib/room_lib.h"
#undef RoomLib_ResetAndSignal_801914F8
#undef func_80191158

extern int RoomLib_ResetAndSignal_801914F8(RoomEnt *obj);

#define ROOMLIB_ADVANCE_ARC_NAME func_80191158
#define ROOMLIB_ADVANCE_ARC_RESET RoomLib_ResetAndSignal_801914F8
#include "../room_lib/RoomLib_AdvanceArcToTarget.inc"
