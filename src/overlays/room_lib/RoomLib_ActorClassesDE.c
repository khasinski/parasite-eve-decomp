/* MASPSX_FLAGS: --expand-div */
/*
 * Room library, first object: the actor classes driven by RoomLib_HandlerD
 * and RoomLib_HandlerE.
 *
 * 117 room and scene overlays (and 8 more with a longer variant) link the
 * same 53 functions in the same order; this object and
 * RoomLib_ActorClassesBCA.c are that library. The split between them is the
 * zero word between the HandlerE and HandlerB jump tables in rodata, which
 * one object cannot produce. Each class occupies the seven-method run its
 * room class table lists (no-op, Init, Configure, no-op, Update, ...,
 * Release, no-op), with the class's private states between Update and
 * Release.
 */
#include "room_lib.h"
#include "handler_e_family.h"

ROOMLIB_RETURN_ZERO(RoomLib_HandlerDNop0)
ROOMLIB_INIT_B(RoomLib_InitHandlerD, RoomLib_HandlerD)
ROOMLIB_HANDLER_D_ARGS(RoomLib_ConfigureHandlerD, RoomLib_ArmHandlerD)
ROOMLIB_RETURN_ZERO(RoomLib_HandlerDNop3)
ROOMLIB_STATE_DISPATCH_VARIANT2(RoomLib_UpdateHandlerD, RoomLib_ReleaseHandlerD)
ROOMLIB_NOTIFY_AND_ARM_B(RoomLib_ArmHandlerD, RoomLib_HandlerD)

#define ROOMLIB_HANDLER_D_ROT_TABLE D_800966EC
#define ROOMLIB_HANDLER_D_TRANSFORM RoomLib_HandlerDTransformTarget
#define ROOMLIB_HANDLER_D_RESET RoomLib_ReleaseHandlerD
#include "RoomLib_HandlerD.inc"

ROOMLIB_FX_NOTIFY(RoomLib_FxNotify)
ROOMLIB_ROTATE_MOTION(RoomLib_HandlerDTransformTarget)
ROOMLIB_RESET_SIGNAL_WITH_TARGET_GATE(RoomLib_ReleaseHandlerD)
ROOMLIB_RETURN_ZERO(RoomLib_HandlerDNop6)

ROOMLIB_RETURN_ZERO(RoomLib_HandlerENop0)
ROOMLIB_INIT_D(RoomLib_InitHandlerE, RoomLib_HandlerE)
ROOMLIB_HANDLER_E_ARGS(RoomLib_ConfigureHandlerE, RoomLib_ArmHandlerE)
ROOMLIB_RETURN_ZERO(RoomLib_HandlerENop3)
ROOMLIB_STATE_DISPATCH_VARIANT2(RoomLib_UpdateHandlerE, RoomLib_ReleaseHandlerE)
ROOMLIB_NOTIFY2_AND_ARM_B_VIA(RoomLib_ArmHandlerE, RoomLib_HandlerE,
                              RoomLib_HandlerESteerToward)
ROOMLIB_HANDLER_E(RoomLib_HandlerE, RoomLib_ReleaseHandlerE,
                  RoomLib_HandlerESteerToward, D_800966EC)
ROOMLIB_STEER_TOWARD(RoomLib_HandlerESteerToward)
ROOMLIB_RESET_AND_SIGNAL_B(RoomLib_ReleaseHandlerE)
ROOMLIB_RETURN_ZERO(RoomLib_HandlerENop6)
