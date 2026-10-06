/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -fno-expensive-optimizations -fcall-used-$1 */
/* Psy-Q LIBDS DSREADY.OBJ, part 4 of 5: ER_retry. */
#include "pe1/psyq_cd.h"
#include "pe1/psyq_ds_queue.h"
#include "pe1/cdrom.h"

extern int D_8009B6EC;
extern int DsPosToInt(CdlLOC *);
extern int func_8007FC44(void);
extern void func_8008227C(void);

int ER_retry(void) {
    int mode;
    CdlLOC *position;
    int command;
    int argumentMode;
    register int limit asm("$3");

    DsReadyCallback(0);
    D_8009B6EC = DsPosToInt(DS_lastpos());
    mode = DS_lastmode() & 0xFF;
    position = DS_lastpos();
    command = func_8007FC44();
    argumentMode = mode;
    limit = -1;
    return DsPacket(argumentMode, position,
        (unsigned char)command, (DslCB)func_8008227C, limit);
}
