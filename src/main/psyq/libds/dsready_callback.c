/* ASSEMBLER: GNU */
#include "pe1/psyq_cd.h"
extern DsAsyncReadState D_8009B6EC;
extern int D_8009B6F8, D_8009B70C;
extern DsEventCallback D_8009B700, D_8009B704;
CdlLOC *CdRom_GetCurrentPosPtr(void);
int CdPosToInt(CdlLOC *);
int CdRom_GetCmdMode(void);
int CdRom_GetPendingReadCount(void);
int CdRom_RestartSeek(void);
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
    state = &D_8009B6EC;
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
            register DsAsyncReadState *current = &D_8009B6EC;
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
        register int *pending asm("$16") = &D_8009B6F8;
        asm("" : "=r"(pending) : "0"(pending));
        if (*pending == 1) {
            if (ASYNC_FROM_RETRY(pending)->retriesRemaining > 0 ||
                ASYNC_FROM_RETRY(pending)->retriesRemaining == -1) {
                CdRom_RestartSeek();
                if (ASYNC_FROM_RETRY(pending)->retriesRemaining > 0)
                    --ASYNC_FROM_RETRY(pending)->retriesRemaining;
            } else {
                if (D_8009B70C == *pending) {
                    DsSyncCallback(D_8009B700);
                    DsReadyCallback(D_8009B704);
                    Render_AllocParticleNode(9, 0, 0, -1);
                }
                D_8009B70C = 0;
                if (ASYNC_FROM_RETRY(pending)->callback)
                    ASYNC_FROM_RETRY(pending)->callback(5, result, detail);
            }
            D_8009B6F8 = 0;
        }
    }
done:;
}
