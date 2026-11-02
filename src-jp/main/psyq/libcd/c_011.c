/* ASSEMBLER: GNU */
/* Psy-Q LIBCD C_011.OBJ, part 1 of 2: StCdInterrupt and mem2mem. C_011 stays
 * in two units: these match with GCC 2.7.2, dma_execute (c_011_2.c) only with
 * GCC 2.8.1, -mno-split-addresses and -fno-schedule-insns. */

#include "pe1/psyq_cd.h"

#include "c_011_externs.h"

/* Separate aliases retain the retail handler's address-load schedule. */

/* Streaming comparisons deliberately use only the low 16 bits of frameCount. */
#define ST_FRAME_INDEX(header) (*(u16 *)&(header)->frameCount)

void StCdInterrupt(void) {
    /* The two status halfwords and the two eight-byte CD buffers share this frame. */
    struct {
        u16 reserved;
        volatile u16 status, track;
        u16 pad;
        u8 subheader[8], response[8];
    } w;
    s32 headerWord;
    StHEADER *header;
    u32 *copyDestination;
    u32 *copySource;
    register u32 frame;
    register u32 subheaderIndex;
    register u32 copyIndex asm("$4");
    register u32 dmaControl;
    u8 *subheaderByte;

    if (g_CdStreamDataReadyFlag != 1) {
        if ((g_StRgb24Mode != 0) && (*g_CdStIntrMdecDmaChcrPtr & 0x01000000)) {
            g_CdStreamReady.word = 1;
            if (g_CdStreamMemorySource != 0) {
                g_CdStreamSectorSlot += 1;
            }
            g_CdStIntrState = 1;
            return;
        }
        if (CdReady(1, w.response) != 5) {
            w.status = w.response[0];
            w.track = w.response[1];
            if (w.status & 4) {
                g_CdStIntrState = 3;
                return;
            }
            header = StRingAddr + g_CdStreamRingWriteSlot;
            g_StCurrentHeader = header;
            asm volatile("" ::: "memory");
            if (header->id != 0) {
                if (g_CdStreamMemorySource != 0) {
                    g_CdStreamSectorSlot += 1;
                }
                g_CdStIntrState = 4;
                return;
            }
            *g_CdStIntrIndexRegPtr = 0;
            *g_CdStIntrRequestRegPtr = 0;
            *g_CdStIntrIndexRegPtr = 0;
            *g_CdStIntrRequestRegPtr = 0x80;
            *g_CdStIntrDelayRegPtr = 0x20943;
            *g_CdStIntrComDelayRegPtr = 0x1323;
            subheaderIndex = 0;
            if (g_DsStreamNoLocFlag == 0) {
                register u8 *sub = w.subheader;
                do {
                    subheaderByte = sub + subheaderIndex;
                    subheaderIndex += 1;
                    *subheaderByte = *g_CdStIntrDataFifoPtr;
                } while (subheaderIndex < 4U);
                copyIndex = 0;
                do {
                    *g_CdStIntrDataFifoPtr;
                    copyIndex += 1;
                } while (copyIndex < 8U);
            }
            dmaControl = 0x11000000;
            if (g_CdStreamMemorySource != 0) {
                mem2mem(g_StCurrentHeader, g_CdStreamMemorySource + (g_CdStreamSectorSlot << 0xB), 8, 0);
            } else {
                dma_execute(3, g_StCurrentHeader, 0, 8, dmaControl, 0U, 0);
            }
            if (*g_CdStIntrDmaChcrPtr & 0x01000000) {
                do {

                } while (*g_CdStIntrDmaChcrPtr & 0x01000000);
            }
            {
                register u32 control = 0x20843;
                register StHEADER *header = g_StCurrentHeader;
                register volatile u32 *port = g_CdStIntrDelayRegPtr;
                header->loc = *(CdlLOC *)w.subheader;
                *port = control;
            }
            *g_CdStIntrComDelayRegPtr = 0x1325;
            if ((g_CdStreamMask == 1) && (g_CdStreamStartFrame != 0)) {
                if (g_CdStreamStartFrame != ST_FRAME_INDEX(g_StCurrentHeader)) {
                    g_StCurrentHeader->id = 0;
                    asm volatile("" ::: "memory");
                    if (g_CdStreamMemorySource != 0) {
                        g_CdStreamSectorSlot += 1;
                    }
                    return;
                } else {
                    g_CdStreamMask = 0;
                }
            }
                if ((g_StCurrentHeader->id != 0x160) ||
                    ((((u16)g_StCurrentHeader->type >> 0xA) & 0x1F) != g_CdStreamExpectedTypeId)) {
                    if (g_CdStreamMemorySource != 0) {
                        g_CdStreamSectorSlot = 0;
                    } else {
                        (void)*(volatile u16 *)&g_StCurrentHeader->id;
                    }
                    g_CdStIntrState = 5;
                    g_StCurrentHeader->id = 0;
                    return;
                }
                if ((g_StFrameSectorIndex != g_StCurrentHeader->secCount) ||
                    ((g_StCurrentFrameNumber != 0) && (g_StCurrentFrameNumber != ST_FRAME_INDEX(g_StCurrentHeader)))) {
                    g_StCurrentFrameNumber = 0;
                    g_StFrameSectorIndex = 0;
                    init_ring_status(g_CdStreamRingReadSlot, g_CdStreamRingWriteSlot - g_CdStreamRingReadSlot);
                    g_CdStreamRingWriteSlot = g_CdStreamRingReadSlot;
                    g_StCurrentHeader->id = 0;
                    asm volatile("" ::: "memory");
                    if (g_CdStreamMemorySource != 0) {
                        g_CdStreamSectorSlot += 1;
                    }
                    g_CdStIntrState = 6;
                    return;
                }
                if (g_StCurrentHeader->secCount == 0) {
                    register u32 limit;
                    frame = ST_FRAME_INDEX(g_StCurrentHeader);
                    limit = g_CdStreamEndState.endSector;
                    g_StFrameSectorIndex = 0;
                    frame &= 0xffff;
                    g_StCurrentFrameNumber = frame;
                    if ((limit != 0) && (frame >= limit)) {
                        g_StCurrentFrameNumber = 0;
                        g_StFrameSectorIndex = 0;
                        init_ring_status(g_CdStreamRingReadSlot, g_CdStreamRingWriteSlot - g_CdStreamRingReadSlot);
                        g_CdStreamRingWriteSlot = g_CdStreamRingReadSlot;
                        g_StCurrentHeader->id = 0;
                        asm volatile("" ::: "memory");
                        g_CdStreamMask = 1;
                        if (g_StrEndCallback != 0) {
                            g_StrEndCallback();
                        }
                        if (g_CdStreamMemorySource != 0) {
                            g_CdStreamSectorSlot += 1;
                        }
                        g_CdStIntrState = 7;
                        return;
                    }
                    if (((StRingSize - g_CdStreamRingWriteSlot) - 1) <
                        (u16)g_StCurrentHeader->nSectors) {
                        if (g_CdStreamEndState.endSector == 0) {
                            g_StCurrentHeader->id = 1;
                            asm volatile("" ::: "memory");
                            g_CdStreamMask = 1;
                            if (g_StrEndCallback != 0) {
                                g_StrEndCallback();
                            }
                            if (g_CdStreamMemorySource != 0) {
                                g_CdStreamSectorSlot += 1;
                            }
                            g_CdStIntrState = 8;
                            return;
                        }
                        if ((s16)StRingAddr->id != 0) {
                            g_StCurrentHeader->id = 0;
                            asm volatile("" ::: "memory");
                            if (g_CdStreamMemorySource != 0) {
                                g_CdStreamSectorSlot += 1;
                            }
                            g_CdStIntrState = 9;
                            return;
                        }
                        g_StCurrentHeader->id = 1;
                        asm volatile("" ::: "memory");
                        copyDestination = (u32 *)StRingAddr;
                        copySource = (u32 *)g_StCurrentHeader;
                        copyIndex = 0;
                        g_CdStreamRingWriteSlot = 0;
                        do {
                            headerWord = *copySource;
                            copySource++;
                            copyIndex += 1;
                            *copyDestination = headerWord;
                            copyDestination++;
                        } while (copyIndex < 8U);
                        g_StCurrentHeader = StRingAddr;
                    }
                begin_frame:
                    g_CdStreamRingReadSlot = g_CdStreamRingWriteSlot;
                }
            transfer_sector:
                g_CdStIntrState = 0xA;
                g_StFrameSectorIndex = (u16)g_StFrameSectorIndex + 1;
                g_CdStreamSectorPayloadPtr =
                    &((StSectorPayload *)(StRingAddr + StRingSize))[g_CdStreamRingWriteSlot];
                dmaControl = 0x11000000;
                if (g_StRgb24Mode != 0) {
                    *g_CdStIntrDelayRegPtr = 0x20943;
                    *g_CdStIntrComDelayRegPtr = 0x1323;
                } else {
                    register u32 value = 0x21020843;
                    register u32 *port;
                    dmaControl = 0x11400000;
                    port = (u32 *)g_CdStIntrDelayRegPtr;
                    *port = value;
                    dmaControl |= 0x100;
                }
                if ((g_StCurrentHeader->nSectors - 1) == g_StCurrentHeader->secCount) {
                    g_CdStreamDataReadyFlag = 1;
                    if (g_CdStreamMemorySource != 0) {
                        mem2mem(g_CdStreamSectorPayloadPtr, g_CdStreamMemorySource + (g_CdStreamSectorSlot << 0xB) + 0x20,
                                0x1F8, 1);
                        g_CdStreamSectorSlot += 1;
                    } else {
                        dma_execute(3, g_CdStreamSectorPayloadPtr, 0, 0x1F8, dmaControl, 1U, 0);
                    }
                    g_StFrameSectorIndex = 0;
                    g_StCurrentFrameNumber = 0;
                    g_CdStreamExpectedTypeId = D_800B63D4;
                } else if (g_CdStreamMemorySource != 0) {
                    mem2mem(g_CdStreamSectorPayloadPtr, g_CdStreamMemorySource + (g_CdStreamSectorSlot << 0xB) + 0x20, 0x1F8,
                            0);
                    g_CdStreamSectorSlot += 1;
                } else {
                    dma_execute(3, g_CdStreamSectorPayloadPtr, 0, 0x1F8, dmaControl, 0U, 0);
                }
                *g_CdStIntrComDelayRegPtr = 0x1325;
                g_StCurrentHeader->id = 3;
                g_CdStreamRingWriteSlot += 1;
                if ((g_CdStreamMemorySource != 0) && (g_CdStreamDataReadyFlag != 0)) {
                    data_ready_callback();
                }
        }
    }
}

void mem2mem(void *destination, void *source, unsigned int count, int reserved) {
    int *dst = destination;
    int *src = source;
    unsigned int i;
    int value;

    i = 0;
    if (count != 0) {
        do {
            value = *src;
            src++;
            i++;
            *dst = value;
            dst++;
        } while (i < count);
    }
}
