/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */
/* Psy-Q LIBDS DSSYS_2.OBJ: CQ_vsync_system. */

#include "pe1/psyq_cd.h"
#include "pe1/psyq_ds.h"

extern s32 D_800A3604;

int DS_system_status(int mode);
void DS_cw(u8 opcode, void *arg);

void CQ_vsync_system(void) {
    int status;
    s32 *pending;
    s32 *workAddress;
    s32 work;
    unsigned int result;

    status = DS_system_status(0);
    if (status == 1) {
        pending = &g_CdPendingReadCount;
        if (*pending > 0) {
            result = DS_system_status(0);
            if (result == status) {
                workAddress = &D_800A3604;
                work = *workAddress;
                result = work * 3;
                result <<= 3;
                work = (s32)CD_DS_QUEUE_FROM_PENDING(pending);
                work = result + work;
                if (((CdDsReadQueueEntry *)work)->active != 0) {
                    DS_cw(((CdDsReadQueueEntry *)work)->command,
                                      ((CdDsReadQueueEntry *)work)->parameter);
                }
            }
        }
    }
}
