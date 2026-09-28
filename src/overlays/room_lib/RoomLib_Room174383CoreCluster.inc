/* func_80193488 */
void func_80193488(void) {
}

/* func_80193490 */
void func_80193490(void) {
}

/* func_80193498 */
void func_80193498(void) {
}

/* func_801934A0 */
void func_801934A0(void* arg0, void* arg1, unsigned char* arg2) {
    unsigned int i;

    *(int*)arg2 = 0;

    for (i = 0; i < 8; i++) {
        *(arg2 + i + 0xB4) = 0;
    }
}

/* RoomLib_DrawEightScaledParticles_801934C8 */
#define ROOMLIB_DRAW_EIGHT_SCALED_NAME func_801934C8
#define ROOMLIB_DRAW_EIGHT_SCALED_TABLE D_80196F0C
#define ROOMLIB_DRAW_EIGHT_SCALED_DEPTH D_8019742A
#include "../room_lib/RoomLib_DrawEightScaledParticles.inc"

/* RoomLib_UpdateEightParticles_80193654 */
#define ROOMLIB_UPDATE_EIGHT_NAME func_80193654
#define ROOMLIB_UPDATE_EIGHT_SEED0 D_8018F064
#define ROOMLIB_UPDATE_EIGHT_SEED1 D_8018F06C
#define ROOMLIB_UPDATE_EIGHT_SEED2 D_8018F074
#include "../room_lib/RoomLib_UpdateEightParticles.inc"

/* func_80193960 */
int func_80193960(void) {
    return 0;
}

/* RoomLib_InitB_80193968 */
#include "../room_lib/room_lib.h"

ROOMLIB_INIT_B(RoomLib_InitB_80193968, RoomLib_HandlerD)

/* RoomLib_HandlerDArgs_80193A08 */
#include "../room_lib/room_lib.h"

ROOMLIB_HANDLER_D_ARGS(RoomLib_HandlerDArgs_80193A08,
                       RoomLib_NotifyArmB_80193D1C)

/* func_80193C8C */
int func_80193C8C(void) {
    return 0;
}

/* func_80193C94 */
#include "../room_lib/room_lib.h"

extern int func_8019462C(RoomEnt *o);

ROOMLIB_STATE_DISPATCH_VARIANT2(func_80193C94, func_8019462C)

/* RoomLib_NotifyArmB_80193D1C */
#include "../room_lib/room_lib.h"

ROOMLIB_NOTIFY_AND_ARM_B(RoomLib_NotifyArmB_80193D1C, RoomLib_HandlerD)

/* RoomLib_HandlerD */
#define ROOMLIB_HANDLER_D_ROT_TABLE D_800966EC
#define ROOMLIB_HANDLER_D_TRANSFORM func_801943EC
#define ROOMLIB_HANDLER_D_RESET func_8019462C
#include "../room_lib/RoomLib_HandlerD.inc"

/* RoomLib_FxNotify */
#include "../room_lib/room_lib.h"

ROOMLIB_FX_NOTIFY(RoomLib_FxNotify)

/* RoomLib_RotateMotion_801943EC */
#include "../room_lib/room_lib.h"

ROOMLIB_ROTATE_MOTION(RoomLib_RotateMotion_801943EC)

/* func_8019462C */
#include "../room_lib/room_lib.h"

ROOMLIB_RESET_SIGNAL_WITH_TARGET_GATE(func_8019462C)

/* func_801946C8 */
int func_801946C8(void) {
    return 0;
}

/* func_801946D0 */
int func_801946D0(void) {
    return 0;
}

/* RoomLib_InitD_801946D8 */
#include "../room_lib/room_lib.h"

ROOMLIB_INIT_D(RoomLib_InitD_801946D8, RoomLib_HandlerE)

/* RoomLib_HandlerEArgs_80194754 */
#include "../room_lib/room_lib.h"

ROOMLIB_HANDLER_E_ARGS(RoomLib_HandlerEArgs_80194754,
                       RoomLib_Notify2ArmB_80194A5C)

/* func_801949CC */
int func_801949CC(void) {
    return 0;
}

/* func_801949D4 */
#include "../room_lib/room_lib.h"

extern int RoomLib_ResetAndSignalB_80195114(RoomEnt *o);

ROOMLIB_STATE_DISPATCH_VARIANT2(func_801949D4, RoomLib_ResetAndSignalB_80195114)

/* RoomLib_Notify2ArmB_80194A5C */
#include "../room_lib/room_lib.h"

ROOMLIB_NOTIFY2_AND_ARM_B(RoomLib_Notify2ArmB_80194A5C, RoomLib_HandlerE)

/* RoomLib_HandlerE */
#include "../room_lib/handler_e_family.h"

ROOMLIB_HANDLER_E(RoomLib_HandlerE, RoomLib_ResetAndSignalB_80195114,
                  RoomLib_SteerToward_80195090, D_800966EC)

/* RoomLib_SteerToward_80195090 */
#include "../room_lib/room_lib.h"

ROOMLIB_STEER_TOWARD(RoomLib_SteerToward_80195090)

/* RoomLib_ResetAndSignalB_80195114 */
#include "../room_lib/room_lib.h"

ROOMLIB_RESET_AND_SIGNAL_B(RoomLib_ResetAndSignalB_80195114)

/* func_8019516C */
int func_8019516C(void) {
    return 0;
}

/* func_80195174 */
int func_80195174(void) {
    return 0;
}

/* RoomLib_InitC_8019517C */
#include "../room_lib/room_lib.h"

ROOMLIB_INIT_C(RoomLib_InitC_8019517C, RoomLib_HandlerB)

/* RoomLib_HandlerBArgs_80195218 */
#include "../room_lib/room_lib.h"

ROOMLIB_HANDLER_B_ARGS(RoomLib_HandlerBArgs_80195218,
                       RoomLib_ArmWindowA_80195490,
                       func_80195894)

/* func_80195460 */
int func_80195460(void) {
    return 0;
}

/* RoomLib_InvokeCallbackC_80195468 */
#include "../room_lib/room_lib.h"

ROOMLIB_INVOKE_CALLBACK_C(RoomLib_InvokeCallbackC_80195468)

/* RoomLib_ArmWindowA_80195490 */
#include "../room_lib/room_lib.h"

ROOMLIB_ARM_IF_WINDOW_A(RoomLib_ArmWindowA_80195490, RoomLib_HandlerB)

/* RoomLib_HandlerB */
#define ROOMLIB_HANDLER_B_ACTOR_PTR D_8009D254
#define ROOMLIB_HANDLER_B_ROT_TABLE D_800966EC
#define ROOMLIB_HANDLER_B_PHASE func_8019588C
#define ROOMLIB_HANDLER_B_RESET RoomLib_ResetAndSignal_80195C34
#include "../room_lib/RoomLib_HandlerB.inc"

/* func_8019588C */
void func_8019588C(void) {
}

/* func_80195894 */
#define func_80195894 func_80195894_header_decl
#define RoomLib_ResetAndSignal_80195C34 RoomLib_ResetAndSignal_80195C34_header_decl
#include "../room_lib/room_lib.h"
#undef RoomLib_ResetAndSignal_80195C34
#undef func_80195894

extern int RoomLib_ResetAndSignal_80195C34(RoomEnt *obj);

#define ROOMLIB_ADVANCE_ARC_NAME func_80195894
#define ROOMLIB_ADVANCE_ARC_RESET RoomLib_ResetAndSignal_80195C34
#include "../room_lib/RoomLib_AdvanceArcToTarget.inc"

/* RoomLib_ResetAndSignal_80195C34 */
#include "../room_lib/room_lib.h"

ROOMLIB_RESET_AND_SIGNAL(RoomLib_ResetAndSignal_80195C34)

/* func_80195C90 */
int func_80195C90(void) {
    return 0;
}

/* func_80195C98 */
int func_80195C98(void) {
    return 0;
}

/* RoomLib_InitFull_80195CA0 */
#include "../room_lib/room_lib.h"

ROOMLIB_INIT_FULL(RoomLib_InitFull_80195CA0, RoomLib_HandlerC)

/* RoomLib_HandlerCArgs_80195D30 */
#include "../room_lib/room_lib.h"

ROOMLIB_HANDLER_C_ARGS(RoomLib_HandlerCArgs_80195D30,
                       RoomLib_ArmWindowB_80195FA8,
                       func_8019649C)

/* func_80195F78 */
int func_80195F78(void) {
    return 0;
}

/* RoomLib_InvokeCallbackC_80195F80 */
#include "../room_lib/room_lib.h"

ROOMLIB_INVOKE_CALLBACK_C(RoomLib_InvokeCallbackC_80195F80)

/* RoomLib_ArmWindowB_80195FA8 */
#include "../room_lib/room_lib.h"

ROOMLIB_ARM_IF_WINDOW_B(RoomLib_ArmWindowB_80195FA8, RoomLib_HandlerC)

/* RoomLib_HandlerC */
#define ROOMLIB_HANDLER_C_ACTOR_PTR D_8009D254
#define ROOMLIB_HANDLER_C_ROT_TABLE D_800966EC
#define ROOMLIB_HANDLER_C_PHASE func_80196494
#define ROOMLIB_HANDLER_C_RESET RoomLib_ResetAndSignal_80196824
#include "../room_lib/RoomLib_HandlerC.inc"

/* func_80196494 */
void func_80196494(void) {
}

/* func_8019649C */
#include "../room_lib/room_lib.h"

extern int RoomLib_ResetAndSignal_80196824(RoomEnt *obj);

#define ROOMLIB_ADVANCE_ARC_Y_NAME func_8019649C
#define ROOMLIB_ADVANCE_ARC_Y_RESET RoomLib_ResetAndSignal_80196824
#include "../room_lib/RoomLib_AdvanceArcToTargetY.inc"

/* RoomLib_ResetAndSignal_80196824 */
#include "../room_lib/room_lib.h"

ROOMLIB_RESET_AND_SIGNAL(RoomLib_ResetAndSignal_80196824)

/* func_80196880 */
int func_80196880(void) {
    return 0;
}

/* func_80196888 */
int func_80196888(void) {
    return 0;
}

/* RoomLib_InitTimers_80196890 */
#include "../room_lib/room_lib.h"

ROOMLIB_INIT_TIMERS(RoomLib_InitTimers_80196890)

/* func_801968CC */
#include "../room_lib/room_lib.h"

extern void RoomLib_RearmOnMatch_80196994(RoomEnt *o);

ROOMLIB_ARG_DISPATCH_REARM(func_801968CC, RoomLib_RearmOnMatch_80196994)

/* func_80196964 */
#include "../room_lib/room_lib.h"

ROOMLIB_RETURN_ZERO(func_80196964)

/* RoomLib_InvokeCallbackC_8019696C */
#include "../room_lib/room_lib.h"

ROOMLIB_INVOKE_CALLBACK_C(RoomLib_InvokeCallbackC_8019696C)

/* RoomLib_RearmOnMatch_80196994 */
#include "../room_lib/room_lib.h"

ROOMLIB_REARM_ON_MATCH(RoomLib_RearmOnMatch_80196994)

/* RoomLib_HandlerA */
#include "../room_lib/room_lib.h"

extern char *D_8009D254;
extern void RoomLib_MoveActorLocal_80196AD8(RoomEnt *obj);

ROOMLIB_FACE_ACTOR_WITH_GLOBALS(RoomLib_HandlerA, RoomLib_MoveActorLocal_80196AD8, D_8009D254, D_800966EC)

/* RoomLib_MoveActorLocal_80196AD8 */
#include "../room_lib/room_lib.h"

ROOMLIB_MOVE_ACTOR_LOCAL(RoomLib_MoveActorLocal_80196AD8, RoomLib_Set4ClearSignal_80196C58)

/* RoomLib_Set4ClearSignal_80196C58 */
#include "../room_lib/room_lib.h"

ROOMLIB_SET4_CLEAR_SIGNAL(RoomLib_Set4ClearSignal_80196C58)

/* func_80196C74 */
int func_80196C74(void) {
    return 0;
}
