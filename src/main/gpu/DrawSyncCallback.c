/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */

#include "common.h"

extern char D_80011854[];
extern void (*D_80095748)(char *message, void *callback);
extern u8 D_8009574E[];

void *DrawSyncCallback(void *callback) {
    u8 *state = D_8009574E;
    void *previous;

    if (state[0] >= 2) {
        D_80095748(D_80011854, callback);
    }

    previous = *(void **)(state + 10);
    *(void **)(state + 10) = callback;
    return previous;
}
