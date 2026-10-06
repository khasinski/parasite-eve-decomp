/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */

#include "pe1/psyq_cd.h"
#include "pe1/psyq_ds.h"

void rescpy(u8 *destination, u8 *source);

void CQ_add_result(int value, int command, u8 *payload) {
    int *cursor;
    u8 *payloadBase;

    cursor = &g_DsReadCallbackCursor;
    asm volatile("" : "+r"(cursor));

    g_DsReadCallbackSlots[*cursor].value = value;
    g_DsReadCallbackSlots[*cursor].command = command;

    payloadBase = (u8 *)cursor - 123;
    rescpy((u8 *)((*cursor << 4) + (int)payloadBase), payload);

    (*cursor)++;
    if (*cursor >= 8) {
        *cursor = 0;
    }
}
