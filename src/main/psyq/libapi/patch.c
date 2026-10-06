/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */
/* PSY-Q LIBAPI PATCH, part 1 of 2: EnablePAD, DisablePAD. */

#include "pe1/psyq_api_internal.h"

void EnablePAD(void) {
    register PadToggleFunc callback asm("$9");

    callback = jtbl_800A34C8;
        goto *(void *)callback;
}

void DisablePAD(void) {
    register PadToggleFunc callback asm("$9");

    callback = jtbl_800A34CC;
        goto *(void *)callback;
}
