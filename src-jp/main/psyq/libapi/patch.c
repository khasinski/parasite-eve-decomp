/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */
/* PSY-Q LIBAPI PATCH, part 1 of 2: EnablePAD, DisablePAD. */
/* LIBAPI PATCH stays in two units: _patch_pad (patch_2.c) is SDK
 * assembler source reported as original_asm, and a unit may not mix that
 * with the C functions EnablePAD and DisablePAD. */

#include "pe1/psyq_api_internal.h"

static inline void PadToggle(PadToggleFunc target) {
    register PadToggleFunc callback asm("$9") = target;
    goto *(void *)callback;
}

void EnablePAD(void) {
    PadToggle(jtbl_800A2F88);
}

void DisablePAD(void) {
    PadToggle(jtbl_800A2F8C);
}
