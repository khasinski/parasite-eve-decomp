/* ASSEMBLER: GNU */
/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */
/* Psy-Q LIBGPU SYS.OBJ part: ClearOTagR, DrawPrim, DrawOTag, PutDrawEnv,
 * DrawOTagEnv and GetDrawEnv.
 * The other SYS.OBJ functions are in neighbouring units because their
 * reconstructions need different compiler options or conflicting
 * declarations.
 */
#include "common.h"
#include "pe1/gpu_callbacks.h"
#include "pe1/gpu_state.h"
#include "pe1/psyq_gpu.h"

extern char D_80011910[];
extern u32 D_800957F8;
extern u32 D_8009580C;

u32 *ClearOTagR(u32 *ot, int count) {
    u32 mask;
    u32 *result;
    u32 *terminator;
    u32 address;
    u32 command;

    if (D_8009574C.queueState.debugLevel >= 2) {
        D_80095748(D_80011910, ot, count);
    }

    D_80095744->clearOTag(ot, count);

    mask = 0xFFFFFF;
    result = ot;
    PE1_COMPILER_LAUNDER(result);
    terminator = &D_8009580C;
    address = (u32)&D_800957F8;
    address &= mask;
    command = 0x04000000;
    address |= command;
    *terminator = address;
    terminator = (u32 *)((u32)terminator & mask);
    *result = (u32)terminator;
    return result;
}

int DrawPrim(void *primitive) {
    u8 *bytes = primitive;
    int word_count = bytes[3];

    D_80095744->callback3C(0);
    return D_80095744->cb14(bytes + 4, word_count);
}

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

void DrawOTagEnv(void *next, DRAWENV *environment)
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

extern void *memcpy(void *dest, const void *src, unsigned int n);
extern char D_8009575C[];

void *GetDrawEnv(void *arg0) {
    void *(*fn)(void *, const void *, unsigned int);

    fn = memcpy;
    fn(arg0, D_8009575C, 0x5C);
    return arg0;
}
