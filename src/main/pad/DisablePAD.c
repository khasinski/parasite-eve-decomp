/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */

#include "pe1/psyq_api_internal.h"

void DisablePAD(void) {
    register PadToggleFunc callback asm("$9");

    callback = jtbl_800A34CC;
        goto *(void *)callback;
}
