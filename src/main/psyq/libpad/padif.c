/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */
/* PSY-Q LIBPAD PADIF, part 1 of 3: MemCard_ReadByteWithCallbackValue. */
/* PADIF stays in three units: padif_2.c only matches with GCC 2.8.1 and
 * -mno-split-addresses, LIBPAD_PADIF_text_26C (padif_3.c) only with GCC
 * 2.7.2. */

#include "common.h"
#include "pe1/psyq_pad_main.h"

extern int g_MemCardDispatchResult;

int MemCard_ReadByteWithCallbackValue(CardObj *port) {
    g_MemCardDispatchResult = g_MemCardStateDispatchFn(port);
    return _padSioRW(port, -2);
}
