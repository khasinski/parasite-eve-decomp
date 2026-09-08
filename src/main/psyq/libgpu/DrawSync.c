/* GCC_VERSION: 2.8.1 */


#include "include_asm.h"
#include "pe1/gpu_callbacks.h"
#include "pe1/gpu_state.h"

extern char D_80011884[];

int DrawSync(int arg0) {
    register int saved asm("$16");
    void (*fn)(char *, int);
    GpuCallbacks *callbacks;

    saved = arg0;
    if (D_8009574C.queueState.debugLevel >= 2) {
        fn = D_80095748;
        fn(D_80011884, saved);
    }

    callbacks = D_80095744;
    callbacks->callback3C(saved);
}
