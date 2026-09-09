/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */

#include "common.h"
#include "pe1/psyq_api_internal.h"

extern u16 D_800945E4[];
extern u16 D_80094620[];
extern u32 *D_8009566C;
extern u16 *D_80095670;
extern volatile u16 *D_80095674;
extern u32 *D_80095678;

void Sys_ClearWordBuf(void *dst, int count);
int RawData_80074354(void *dst);
void Render_InitSceneGeom(void);
void HookEntryInt(void *entry);
int startIntrVSync(void);
void Sys_HleJumpA0(void *arg);
void ExitCriticalSection(void);

u16 *Sys_InitIntrManager(void) {
    register u16 *state asm("$16");
    u16 *dst;
    volatile u16 *src;
    u16 *flag;
    u32 *ptr;
    u32 *vsyncPtr;
    int result;
    DmaCallbackSetter dmaSetter;

    state = D_800945E4;
    if (state[0] != 0) {
        return 0;
    }

    dst = D_80095670;
    src = D_80095674;
    *src = 0;
    *dst = *src;
    *D_80095678 = 0x33333333;

    Sys_ClearWordBuf(state, 0x41A);
    if (RawData_80074354(state + 0x1C) != 0) {
        Render_InitSceneGeom();
    }

    state = D_80094620;
    *((u32 *)state) = (u32)(state + 0x7EE);
    HookEntryInt(state - 2);
    flag = state - 0x1E;
    *flag = 1;

    result = startIntrVSync();
    vsyncPtr = D_8009566C;
    vsyncPtr[5] = result;

    dmaSetter = startIntrDMA();
    ptr = D_8009566C;
    ((DmaCallbackSetter *)ptr)[1] = dmaSetter;

    Sys_HleJumpA0(D_8009566C);
    state = flag;
    ExitCriticalSection();

    return state;
}
