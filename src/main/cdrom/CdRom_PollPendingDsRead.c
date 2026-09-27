/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */

#include "pe1/psyq_cd.h"

extern s32 D_800A3604;

int DsSync(int mode);
void CdRom_TryIssueCmd(u8 opcode, void *arg);

void CdRom_PollPendingDsRead(void) {
    int status;
    s32 *pending;
    s32 *workAddress;
    s32 work;
    unsigned int result;

    status = DsSync(0);
    if (status == 1) {
        pending = &g_CdPendingReadCount;
        if (*pending > 0) {
            result = DsSync(0);
            if (result == status) {
                workAddress = &D_800A3604;
                work = *workAddress;
                result = work * 3;
                result <<= 3;
                work = (s32)CD_DS_QUEUE_FROM_PENDING(pending);
                work = result + work;
                if (((CdDsReadQueueEntry *)work)->active != 0) {
                    CdRom_TryIssueCmd(((CdDsReadQueueEntry *)work)->command,
                                      ((CdDsReadQueueEntry *)work)->parameter);
                }
            }
        }
    }
}
