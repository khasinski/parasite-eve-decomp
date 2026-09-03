/* MASPSX_FLAGS: --expand-div */
#include "../room_lib/handler_e_family.h"

extern void func_8019095C(char *entity, char *state);

ROOMLIB_HANDLER_E(RoomLib_HandlerE, func_801909E0,
                  func_8019095C, D_800966EC)
