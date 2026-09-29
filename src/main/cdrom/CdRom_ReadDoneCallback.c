#include "pe1/psyq_cd.h"
#include "pe1/psyq_ds.h"
#include "pe1/cdrom.h"
/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */

extern int D_8009B708;

void CdRom_ReadDoneCallback(unsigned char arg0, unsigned char *arg1) {
    int *state;
    int status;
    void *data;
    DsAsyncReadCallback callback;

    state = &D_8009B708;
    data = arg1;
    if (state[0] == 0) {
        return;
    }
    if (state[1] == 0) {
        return;
    }

    status = arg0;
    if (status == 2) {
        if (CdRom_GetPendingReadCount() == 0) {
            CdRom_RestartSeek();
        }
        return;
    }

    DsSyncCallback(DS_ASYNC_READ_FIELD(state + 1, savedSyncCallback));
    DsReadyCallback(DS_ASYNC_READ_FIELD(state + 1, savedReadyCallback));
    callback = DS_ASYNC_READ_FIELD(state + 1, callback);
    state[1] = 0;
    if (callback != 0) {
        callback(status, data, 0);
    }
}
