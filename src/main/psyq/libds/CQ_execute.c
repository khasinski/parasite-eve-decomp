/* Psy-Q LIBDS DSSYS_2.OBJ: CQ_execute. */
#include "common.h"
#include "pe1/psyq_ds.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

extern s32 g_CdDsReadIndexBase[] __asm__("D_800A3604");

s32 DS_system_status(s32 mode);
s32 DS_cw(s32 cmd, s32 arg);

s32 CQ_execute(void) {
    s32 index;
    register CdDsReadQueueEntry *entry asm("$3");
    register s32 offset asm("$2");
    if (DS_system_status(0) != 1) {
        return 0;
    }

    entry = (CdDsReadQueueEntry *)g_CdDsReadIndexBase;
    index = *(s32 *)entry;
    entry = (CdDsReadQueueEntry *)((u8 *)entry - 0xC4);
    offset = index * 3;
    offset <<= 3;
    entry = (CdDsReadQueueEntry *)((u8 *)entry + offset);

    if (entry->active != 0) {
        return DS_cw(entry->command, entry->parameter) != 0;
    }
    return 0;
}
