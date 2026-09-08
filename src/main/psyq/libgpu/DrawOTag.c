/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */
#include "pe1/psyq_gpu.h"
#include "pe1/gpu_callbacks.h"
#include "pe1/gpu_state.h"

extern char D_80011928[];
extern unsigned char D_8009574E[];
extern char D_8001193C[];
extern char D_80011954[];
void Gpu_SetDrawEnvBack(DR_ENV *, DRAWENV *);
void *memcpy(void *, const void *, unsigned int);

int DrawOTag(void *table)
{
    if (D_8009574C.queueState.debugLevel >= 2) {
        D_80095748(D_80011928, table);
    }
    {
        GpuCallbacks *callbacks = D_80095744;
        register void *argument asm("$5") = table;
        int count;
        /* Keep the dispatch load before the independently materialized zeros. */
        asm volatile("" : "+r"(callbacks) : "r"(argument));
        count = 0;
        asm volatile("" : "+r"(count));
        return callbacks->addque2(callbacks->u18.packet, argument, count, 0);
    }
}

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

void Gpu_PutDrawEnvLinked(void *next, DRAWENV *environment)
{
    unsigned char *state = D_8009574E;
    DR_ENV *packet;
    void *(*copy)(void *, const void *, unsigned int) = memcpy;
    if (state[0] >= 2) {
        D_80095748(D_80011954, next, environment);
    }
    packet = &environment->dr_env;
    Gpu_SetDrawEnvBack(packet, environment);
    packet->tag = (packet->tag & 0xFF000000) | ((unsigned long)next & 0xFFFFFF);
    D_80095744->addque2(D_80095744->u18.packet, packet, 0x40, 0);
    copy(state + 14, environment, 0x5C);
}
