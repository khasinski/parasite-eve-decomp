/* Sprite callback and poll/reset controller share one pool and layout. */
#include "room_m273_effects.h"
typedef GteShortVector Vector;
extern int D_800E27EC, D_800F3428, D_800966EC[];
extern unsigned short D_800F336C, D_800E1204[];
extern unsigned short GetClut(int, int);
extern void func_800CEE20(Vector *, void *, int, int, int, int, int, int, void *);

int func_80197230(int mode, Vector *position) {
    if (mode == 1) {
        if (D_800E27EC >= 8) return 1;
    } else if (mode == 2) {
        int frame = D_800E27EC - 1;
        int kind, palette;
        register int size asm("$18");
        register int sample asm("$2");
        unsigned short clut;
        sample = D_800966EC[(((unsigned int)frame << 9) & 0x3E00) / 4];
        kind = D_800F336C;
        size = sample + 2048;
        palette = D_800E1204[kind];
        clut = GetClut(0, (kind == 4 && D_800F3428) ? palette + 9 : palette + 5);
        func_800CEE20(position, 0, (short)size, (short)size, 102, clut, 1,
            (short)D_800966EC[(((unsigned int)frame << 10) & 0x3C00) / 4] >> 5, 0);
    }
    return 0;
}

#define ROOMLIB_POLL_RESET_POOL_CONTEXT_DECL
#define ROOMLIB_POLL_RESET_POOL_EXPR D_800F33E0->pool
#define ROOMLIB_POLL_RESET_FUNC func_80197360
#define ROOMLIB_POLL_RESET_CALLBACK func_80197230
#define ROOMLIB_POLL_RESET_FLAG D_8019AF69
#define ROOMLIB_POLL_RESET_COUNTER D_8019AF0A
#define ROOMLIB_POLL_RESET_SEED D_8019AF04
#include "../room_lib/RoomLib_PollAndResetActor.inc"
