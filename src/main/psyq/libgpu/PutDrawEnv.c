/* CC1_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */

#include "pe1/psyq_gpu.h"

#include "pe1/gpu_callbacks.h"

extern unsigned char g_GraphDebug[];
extern void (*g_GpuDebugPrintf[])(char *msg, DRAWENV *env);
extern GpuCallbacks *g_GpuCallbacks;
extern char D_8001193C[];

void Gpu_SetDrawEnvBack(DR_ENV *dr_env, DRAWENV *env);
void *memcpy(void *dst, void *src, int n);

DRAWENV *PutDrawEnv(DRAWENV *env) {
    GpuCallbacks *callbacks;

    if (g_GraphDebug[0] >= 2) {
        g_GpuDebugPrintf[0](D_8001193C, env);
    }

    Gpu_SetDrawEnvBack(&env->dr_env, env);
    env->dr_env.tag |= 0xFFFFFF;
    callbacks = g_GpuCallbacks;
    callbacks->addque2(callbacks->u18.cwc, &env->dr_env, 0x40, 0);
    memcpy(g_GraphDebug + 0xE, env, 0x5C);
    return env;
}
