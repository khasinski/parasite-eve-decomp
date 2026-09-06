/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */
#include "pe1/psyq_gpu.h"
#include "pe1/gpu_callbacks.h"
extern unsigned char D_8009574E[];
extern char D_8001193C[];
extern void (*D_80095748)(char *, void *);
extern GpuCallbacks *D_80095744;
void Gpu_SetDrawEnvBack(DR_ENV *, DRAWENV *);
void *memcpy(void *, const void *, unsigned int);

DRAWENV *PutDrawEnv(DRAWENV *environment)
{
    unsigned char *state = D_8009574E;
    DR_ENV *packet;
    void *(*copy)(void *, const void *, unsigned int) = memcpy;
    if (state[0] >= 2) {
        D_80095748(D_8001193C, environment);
    }
    packet = &environment->dr_env;
    Gpu_SetDrawEnvBack(packet, environment);
    packet->tag |= 0xFFFFFF;
    D_80095744->addque2(D_80095744->u18.packet, packet, 0x40, 0);
    copy(state + 14, environment, 0x5C);
    return environment;
}
