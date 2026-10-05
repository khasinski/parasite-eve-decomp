#include "common.h"
#include "pe1/room_fx.h"

#ifndef ROOMLIB_POLL_RESET_FUNC
#error "ROOMLIB_POLL_RESET_FUNC must name the room entry point"
#endif
#ifndef ROOMLIB_POLL_RESET_CALLBACK
#error "ROOMLIB_POLL_RESET_CALLBACK must name the spawned callback"
#endif
#ifndef ROOMLIB_POLL_RESET_FLAG
#error "ROOMLIB_POLL_RESET_FLAG must name the completion flag"
#endif
#ifndef ROOMLIB_POLL_RESET_COUNTER
#error "ROOMLIB_POLL_RESET_COUNTER must name the pending counter"
#endif
#ifndef ROOMLIB_POLL_RESET_SEED
#error "ROOMLIB_POLL_RESET_SEED must name the copied seed"
#endif

#ifndef ROOMLIB_POLL_RESET_POOL_CONTEXT_DECL
#define ROOMLIB_POLL_RESET_POOL_CONTEXT_DECL extern char **D_800F33E0
#endif
#ifndef ROOMLIB_POLL_RESET_POOL_EXPR
#define ROOMLIB_POLL_RESET_POOL_EXPR D_800F33E0[2]
#endif
ROOMLIB_POLL_RESET_POOL_CONTEXT_DECL;
extern u16 D_800E11FA;
extern u16 D_800E2850[];
extern u16 D_800F3368;
extern s16 D_800F336A;
extern u16 D_800F336C;
extern u16 D_800F336E;
extern u16 D_800F3370;
extern u16 D_800F3372;
extern u16 D_800F3374;
extern u16 D_800F3376;
extern u16 D_800F3378;
extern u8 ROOMLIB_POLL_RESET_FLAG;
extern s16 ROOMLIB_POLL_RESET_COUNTER;
extern RoomFxSeed8 ROOMLIB_POLL_RESET_SEED;

int ROOMLIB_POLL_RESET_CALLBACK();
s32 func_800CE560(void *arg0, s32 arg1, s32 arg2, int (*arg3)());
void *func_800CE610(void *arg0);

s32 ROOMLIB_POLL_RESET_FUNC(s32 state) {
    RoomFxSeed8 *obj;
    int index;
    register int work asm("$3");
    register int value2 asm("$2");
    register int value3 asm("$2");
    register int value1 asm("$2");

    switch (state) {
    case 0:
        return func_800CE560(ROOMLIB_POLL_RESET_POOL_EXPR, 8, 4,
                             ROOMLIB_POLL_RESET_CALLBACK);
    case 1:
        if (ROOMLIB_POLL_RESET_FLAG != 0) {
            return 2;
        }
        if (ROOMLIB_POLL_RESET_COUNTER == 0) {
            return 0;
        }
        obj = (RoomFxSeed8 *)func_800CE610(ROOMLIB_POLL_RESET_POOL_EXPR);
        if (obj != 0) {
            ROOMLIB_POLL_RESET_COUNTER = 0;
            *obj = ROOMLIB_POLL_RESET_SEED;
        }
        return 0;
    case 2:
        index = D_800E11FA;
        work = 0x20;
        D_800F3368 = work;
        value2 = 2;
        D_800F336A = value2;
        D_800F3376 = work;
        D_800F3378 = work;
        D_800F3376 = work;
        D_800F3378 = work;
        index <<= 1;
        PE1_COMPILER_USE(work);
        work = *(u16 *)((u8 *)D_800E2850 + index);
        value3 = 3;
        D_800F336C = value3;
        value1 = 1;
        D_800F336E = value1;
        D_800F3372 = 0;
        D_800F3374 = 0;
        D_800F3370 = work;
        PE1_COMPILER_MEMORY_BARRIER();
        return 0;
    }
    return 0;
}

#undef ROOMLIB_POLL_RESET_FUNC
#undef ROOMLIB_POLL_RESET_CALLBACK
#undef ROOMLIB_POLL_RESET_FLAG
#undef ROOMLIB_POLL_RESET_COUNTER
#undef ROOMLIB_POLL_RESET_SEED
#undef ROOMLIB_POLL_RESET_POOL_CONTEXT_DECL
#undef ROOMLIB_POLL_RESET_POOL_EXPR
