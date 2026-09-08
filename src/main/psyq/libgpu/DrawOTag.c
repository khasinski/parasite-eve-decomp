/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */
#include "pe1/gpu_callbacks.h"
#include "pe1/gpu_state.h"

extern char D_80011928[];

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
