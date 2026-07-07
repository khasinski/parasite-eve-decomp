/* CC1_FLAGS: -g3 */

typedef signed short s16;
typedef int s32;

s32 Akao_SendPositionalCmd(void *arg0, s32 arg1, s32 arg2, s16 arg3, s16 arg4, s16 arg5);
extern void *g_AkaoBgmHandle[];

s32 Asset_Find08Alt(s32 arg0, s32 arg1, s16 arg2, s16 arg3, s16 arg4) {
    return Akao_SendPositionalCmd(g_AkaoBgmHandle[0], arg0, arg1, arg2, arg3, arg4);
}
