/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */

#include "pe1/psyq_cd.h"

extern s32 D_800A3604;

int DsSync(int mode);
void CdRom_TryIssueCmd(u8 opcode, void *arg);

void CdRom_PollPendingDsRead(void) {
    register int status asm("$16");
    register s32 *pending asm("$17");
    register s32 *workAddress asm("$3");
    register s32 work asm("$3");
    int result;

    status = DsSync(0);
    if (status == 1) {
        PE1_COMPILER_MEMORY_BARRIER();
        pending = &g_CdPendingReadCount;
        PE1_COMPILER_LAUNDER(pending);
        if (*pending > 0) {
            result = DsSync(0);
            if (result == status) {
                PE1_COMPILER_MEMORY_BARRIER();
                workAddress = &D_800A3604;
                work = *workAddress;
                result = work * sizeof(CdDsReadQueueEntry);
                PE1_COMPILER_LAUNDER(result);
                work = (s32)pending - 0xC8;
                work = result + work;
                if (((CdDsReadQueueEntry *)work)->active != 0) {
                    CdRom_TryIssueCmd(((CdDsReadQueueEntry *)work)->command,
                                      ((CdDsReadQueueEntry *)work)->parameter);
                }
            }
        }
    }
}
