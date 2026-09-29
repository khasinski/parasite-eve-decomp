/* ASSEMBLER: GNU */
#include "pe1/psyq_ds.h"
#include "pe1/cdrom.h"
int CdPosToInt(CdlLOC *);
#define ASYNC_FROM_RETRY(p)                                                            \
    ((DsAsyncReadState *)((u8 *)(p) - PE1_OFFSETOF(DsAsyncReadState, retryPending)))
void LIBDS_DSREADY_text_FC(int inEvent, u8 *inResult) {
    u8 detail[12];
    register int event;
    register u8 *result = inResult;
    register DsAsyncReadState *state asm("$17");
    register int one;
    /* Order prologue saves. This temporary output is used only by the next
     * empty constraint, then replaced with the incoming event. */
    asm("" : "=r"(event));
    state = &g_DsAsyncReadState;
    asm("" : "=r"(state) : "0"(state), "r"(event));
    event = inEvent;
    if (state->nextSector == -1)
        state->nextSector = CdPosToInt(CdRom_GetCurrentPosPtr());
    event = (u8)event;
    one = 1;
    if (event == one) {
        if (CdRom_GetCmdMode() & 32) {
            {
                register DsCallback cb = DsDataCallback(0);
                register void *dest asm("$4") = detail;
                register int count asm("$5") = 3;
                asm("" : : "r"(dest), "r"(count));
                event = (int)cb;
                CdRom_IsBusy(dest, count);
            }
            DsDataCallback((DsCallback)event);
            event = CdPosToInt((CdlLOC *)detail);
            if (event != state->nextSector)
                goto retry;
            if (state->callback && state->lastDeliveredSector < event) {
                state->callback(1, result, detail);
                state->lastDeliveredSector = event;
            }
        } else {
            if (state->callback) {
                register int sector = state->nextSector;
                if (state->lastDeliveredSector < sector) {
                    state->callback(1, result, detail);
                    state->lastDeliveredSector = state->nextSector;
                }
            }
        }
        {
            register DsAsyncReadState *current = &g_DsAsyncReadState;
            current->nextSector++;
        }
    } else if (event == 4) {
        if (state->callback)
            state->callback(4, result, detail);
    } else {
        if (*result & 16) {
            if (state->reserved1C == one) {
                DsSyncCallback(0);
                goto done;
            }
            DsSyncCallback(state->saved_sync_callback);
            DsReadyCallback(state->saved_ready_callback);
            state->active = 0;
            if (state->callback)
                state->callback(event, result, detail);
            goto done;
        } else if (!CdRom_GetPendingReadCount() && !(*result & 160)) {
        retry:
            state->retryPending = one;
        }
    }
    {
        register int *pending asm("$16") = &g_DsAsyncReadRetryPending;
        asm("" : "=r"(pending) : "0"(pending));
        if (*pending == 1) {
            if (ASYNC_FROM_RETRY(pending)->retriesRemaining > 0 ||
                ASYNC_FROM_RETRY(pending)->retriesRemaining == -1) {
                CdRom_RestartSeek();
                if (ASYNC_FROM_RETRY(pending)->retriesRemaining > 0)
                    --ASYNC_FROM_RETRY(pending)->retriesRemaining;
            } else {
                if (g_DsReadBusy == *pending) {
                    DsSyncCallback(g_DsAsyncReadSavedSyncCallback);
                    DsReadyCallback(g_DsAsyncReadSavedReadyCallback);
                    Render_AllocParticleNode(9, 0, 0, -1);
                }
                g_DsReadBusy = 0;
                if (ASYNC_FROM_RETRY(pending)->callback)
                    ASYNC_FROM_RETRY(pending)->callback(5, result, detail);
            }
            g_DsAsyncReadRetryPending = 0;
        }
    }
done:;
}
