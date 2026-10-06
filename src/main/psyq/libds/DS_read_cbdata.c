/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */
/* Psy-Q LIBDS DSREAD.OBJ: DS_read_cbdata. */
#include "pe1/psyq_cd.h"
extern int D_8009B6B0[];
extern int VSync(int);
extern void DsReadBreak(void);

void DS_read_cbdata(void) {
    int *state = D_8009B6B0;
    int query;
    query = -1;
    /* Keep the clock query independent of the sector decrement. */
    asm("" : "+r"(query));
    state[0] += state[-1] * 4;
    state[1]--;
    if (VSync(query) > state[5] + 1200) state[1] = -1;
    if (!state[1] || VSync(-1) > state[5] + 1200) {
        DsReadBreak();
        if (g_CdReadCompleteCallback)
            g_CdReadCompleteCallback(state[1] < 0 ? 5 : 2, (void *)state[3]);
    }
}
