/* ASSEMBLER: GNU */
#include "pe1/psyq_cd.h"
extern volatile u8 *D_8009B334;
extern volatile u8 *D_8009B338;
extern volatile u32 *D_8009B33C;
extern volatile u32 *D_8009B340;
extern volatile u32 *D_8009B34C;
extern volatile u32 *D_8009B35C;
extern s32 D_8009B374;
extern StHEADER *D_800A34A0;
extern u32 D_800A5D54;
extern s16 D_800A8018;
extern s32 D_800A801C;
extern void (*D_800B0CCC)(void);
extern s32 D_800B0CD0, D_800B6914, D_800B8620, D_800B89F4, D_800BCD7C, D_800BE9E4;
extern u32 D_800C0DBC;
extern s32 D_800C0DC0;
extern u8 *D_800C0DC4;
extern StHEADER *D_800C0DC8;
extern s32 D_800C20C4;

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
    register u32 discardIndex asm("$4");
    register u32 copyIndex asm("$4");
    register u32 dmaControl;
    u8 *subheaderByte;

    if (D_800B89F4 != 1) {
        if ((D_800A801C != 0) && (*D_8009B34C & 0x01000000)) {
            D_800B0CD0 = 1;
            if (D_800C0DB8 != 0) {
                D_800BCD7C += 1;
            }
            D_8009B374 = 1;
            return;
        }
        if (CdReady(1, w.response) != 5) {
            w.status = w.response[0];
            w.track = w.response[1];
            if (w.status & 4) {
                D_8009B374 = 3;
                return;
            }
            header = D_800C0DC8 + D_800BE998;
            D_800A34A0 = header;
            asm volatile("" ::: "memory");
            if (header->id != 0) {
                if (D_800C0DB8 != 0) {
                    D_800BCD7C += 1;
                }
                D_8009B374 = 4;
                return;
            }
            *D_8009B32C = 0;
            *D_8009B338 = 0;
            *D_8009B32C = 0;
            *D_8009B338 = 0x80;
            *D_8009B33C = 0x20943;
            *D_8009B340 = 0x1323;
            subheaderIndex = 0;
            if (D_800A8020 == 0) {
                register u8 *sub = w.subheader;
                do {
                    subheaderByte = sub + subheaderIndex;
                    subheaderIndex += 1;
                    *subheaderByte = *D_8009B334;
                } while (subheaderIndex < 4U);
                discardIndex = 0;
                do {
                    *D_8009B334;
                    discardIndex += 1;
                } while (discardIndex < 8U);
            }
            dmaControl = 0x11000000;
            if (D_800C0DB8 != 0) {
                mem2mem(D_800A34A0, D_800C0DB8 + (D_800BCD7C << 0xB), 8, 0);
            } else {
                dma_execute(3, (u32)D_800A34A0, 0, 8, dmaControl, 0U, 0);
            }
            if (*D_8009B35C & 0x01000000) {
                do {

                } while (*D_8009B35C & 0x01000000);
            }
            {
                register u32 control = 0x20843;
                register StHEADER *header = D_800A34A0;
                register volatile u32 *port = D_8009B33C;
                header->loc = *(CdlLOC *)w.subheader;
                *port = control;
            }
            *D_8009B340 = 0x1325;
            if ((D_800C0DC0 == 1) && (D_800B6918 != 0)) {
                if (D_800B6918 != ST_FRAME_INDEX(D_800A34A0)) {
                    D_800A34A0->id = 0;
                    asm volatile("" ::: "memory");
                    if (D_800C0DB8 != 0) {
                        D_800BCD7C += 1;
                    }
                } else {
                    D_800C0DC0 = 0;
                    goto validate_header;
                }
            } else {
            validate_header:
                if ((D_800A34A0->id != 0x160) ||
                    ((((u16)D_800A34A0->type >> 0xA) & 0x1F) != D_800B8620)) {
                    if (D_800C0DB8 != 0) {
                        D_800BCD7C = 0;
                    } else {
                        (void)*(volatile u16 *)&D_800A34A0->id;
                    }
                    asm volatile("" ::: "memory");
                    D_8009B374 = 5;
                    D_800A34A0->id = 0;
                    asm volatile("" ::: "memory");
                    return;
                }
                if ((D_800A8018 != D_800A34A0->secCount) ||
                    ((D_800A5D54 != 0) && (D_800A5D54 != ST_FRAME_INDEX(D_800A34A0)))) {
                    D_800A5D54 = 0;
                    D_800A8018 = 0;
                    init_ring_status(D_800BE9E4, D_800BE998 - D_800BE9E4);
                    D_800BE998 = D_800BE9E4;
                    D_800A34A0->id = 0;
                    asm volatile("" ::: "memory");
                    if (D_800C0DB8 != 0) {
                        D_800BCD7C += 1;
                    }
                    D_8009B374 = 6;
                    return;
                }
                if (D_800A34A0->secCount == 0) {
                    register u32 limit;
                    frame = ST_FRAME_INDEX(D_800A34A0);
                    limit = D_800C0DBC;
                    D_800A8018 = 0;
                    frame &= 0xffff;
                    D_800A5D54 = frame;
                    if ((limit != 0) && (frame >= limit)) {
                        D_800A5D54 = 0;
                        D_800A8018 = 0;
                        init_ring_status(D_800BE9E4, D_800BE998 - D_800BE9E4);
                        D_800BE998 = D_800BE9E4;
                        D_800A34A0->id = 0;
                        asm volatile("" ::: "memory");
                        D_800C0DC0 = 1;
                        if (D_800B0CCC != 0) {
                            D_800B0CCC();
                        }
                        if (D_800C0DB8 != 0) {
                            D_800BCD7C += 1;
                        }
                        D_8009B374 = 7;
                        return;
                    }
                    if ((u32)((D_800C20C4 - D_800BE998) - 1) <
                        (u16)D_800A34A0->nSectors) {
                        if (D_800C0DBC == 0) {
                            D_800A34A0->id = 1;
                            asm volatile("" ::: "memory");
                            D_800C0DC0 = 1;
                            if (D_800B0CCC != 0) {
                                D_800B0CCC();
                            }
                            if (D_800C0DB8 != 0) {
                                D_800BCD7C += 1;
                            }
                            D_8009B374 = 8;
                            return;
                        }
                        if ((s16)D_800C0DC8->id != 0) {
                            D_800A34A0->id = 0;
                            asm volatile("" ::: "memory");
                            if (D_800C0DB8 != 0) {
                                D_800BCD7C += 1;
                            }
                            D_8009B374 = 9;
                            return;
                        }
                        D_800A34A0->id = 1;
                        asm volatile("" ::: "memory");
                        copyDestination = (u32 *)D_800C0DC8;
                        copySource = (u32 *)D_800A34A0;
                        copyIndex = 0;
                        D_800BE998 = 0;
                        do {
                            headerWord = *copySource;
                            copySource++;
                            copyIndex += 1;
                            *copyDestination = headerWord;
                            copyDestination++;
                        } while (copyIndex < 8U);
                        D_800A34A0 = D_800C0DC8;
                        goto begin_frame;
                    }
                begin_frame:
                    D_800BE9E4 = D_800BE998;
                    goto transfer_sector;
                }
            transfer_sector:
                D_8009B374 = 0xA;
                D_800A8018 = (u16)D_800A8018 + 1;
                D_800C0DC4 =
                    (u8 *)D_800C0DC8 + (D_800C20C4 << 5) + (D_800BE998 * 0x7E0);
                dmaControl = 0x11000000;
                if (D_800A801C != 0) {
                    *D_8009B33C = 0x20943;
                    *D_8009B340 = 0x1323;
                } else {
                    register u32 value = 0x21020843;
                    register u32 *port;
                    dmaControl = 0x11400000;
                    port = (u32 *)D_8009B33C;
                    *port = value;
                    dmaControl |= 0x100;
                }
                if ((D_800A34A0->nSectors - 1) == D_800A34A0->secCount) {
                    D_800B89F4 = 1;
                    if (D_800C0DB8 != 0) {
                        mem2mem(D_800C0DC4, D_800C0DB8 + (D_800BCD7C << 0xB) + 0x20,
                                0x1F8, 1);
                        D_800BCD7C += 1;
                    } else {
                        dma_execute(3, (u32)D_800C0DC4, 0, 0x1F8, dmaControl, 1U, 0);
                    }
                    D_800A8018 = 0;
                    D_800A5D54 = 0;
                    D_800B8620 = D_800B6914;
                } else if (D_800C0DB8 != 0) {
                    mem2mem(D_800C0DC4, D_800C0DB8 + (D_800BCD7C << 0xB) + 0x20, 0x1F8,
                            0);
                    D_800BCD7C += 1;
                } else {
                    dma_execute(3, (u32)D_800C0DC4, 0, 0x1F8, dmaControl, 0U, 0);
                }
                *D_8009B340 = 0x1325;
                D_800A34A0->id = 3;
                D_800BE998 += 1;
                if ((D_800C0DB8 != 0) && (D_800B89F4 != 0)) {
                    data_ready_callback();
                }
            }
        }
    }
}
