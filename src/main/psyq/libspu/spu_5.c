/* GCC_VERSION: 2.8.1 */
/* PSY-Q LIBSPU SPU, part 5 of 5: _spu_Fw1ts. */
#include "pe1/psyq_spu_internal.h"

void _spu_Fw1ts(void) {
    volatile int i;
    volatile u32 value;

    value = 13;
    *(int *)&i = 0;
    while (i < 60) {
        value = value * 13;
        i++;
    }
}
