/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */

#include "common.h"
#include "pe1/gpu_state.h"

extern char D_80011854[];

void *DrawSyncCallback(void *callback) {
    void *previous;

    if (D_8009574C.queueState.debugLevel >= 2) {
        D_80095748(D_80011854, callback);
    }

    previous = (void *)D_8009574C.drawSyncCallback;
    D_8009574C.drawSyncCallback = (void (*)())callback;
    return previous;
}
