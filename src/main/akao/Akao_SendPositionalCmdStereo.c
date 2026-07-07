/* CC1_FLAGS: -g3 */

typedef signed short s16;
typedef int s32;

s32 Akao_SendPositionalCmd(void *arg0, s32 arg1, s32 arg2, s16 arg3, s16 arg4, s16 arg5);
extern void *g_AkaoBgmHandle;

s32 Akao_SendPositionalCmdStereo(s32 arg0, s32 arg1, s16 arg2, s16 arg3, s16 arg4) {
    s32 ret;
    void **basep = &g_AkaoBgmHandle;

    ret = Akao_SendPositionalCmd(*basep, arg0, arg1, arg2, arg3, arg4);
    Akao_SendPositionalCmd(*basep, arg0 + 1, arg1, arg2, arg3, arg4);
    return ret;
}
