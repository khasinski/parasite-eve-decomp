/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */

#include "common.h"
#include "pe1/psyq_cd.h"

#define CdlDiskError 5
#define true 1
#define NULL ((void *)0)
#define STREAM_SECTOR_PAYLOAD_SIZE 0x7E0

static volatile u16 *D_800A34A0;
#define CURRENT_STREAM_HEADER ((volatile StHEADER *)D_800A34A0)

extern volatile s32 *D_8009B33C;
extern volatile s32 *D_8009B340;
extern volatile s32 *D_8009B34C;
extern volatile s32 *D_8009B35C;
extern volatile u8 *D_8009B32C;
extern volatile u8 *D_8009B334;
extern volatile u8 *D_8009B338;
extern s32 D_8009B374;
extern s32 D_800A5D54;
extern s16 D_800A8018;
extern s32 D_800A801C;
extern s32 D_800A8020;
extern void (*D_800B0CCC)(void);
extern s32 D_800B0CD0;
extern s32 D_800B6914;
extern s32 D_800B8620;
extern s32 D_800B89F4;
extern s32 D_800BCD7C;
extern s32 D_800BE998;
extern s32 D_800BE9E4;
extern u8 *D_800C0DC4;

int CdReady(int mode, u8 *result);
void init_ring_status(int start, u32 count);
void data_ready_callback(void);
void mem2mem(s32 *dst, s32 *src, u32 count, u32 unused);
void dma_execute(s32 channel, void *address, s32 block_count, s32 block_size,
                 u32 control, u8 interrupt, u32 unused);

typedef struct StCdResponseWords {
    s16 reserved0;
    s16 status;
    s16 detail;
    s16 reserved6;
} StCdResponseWords;

void StCdInterrupt(void) {
    volatile StCdResponseWords response_words;
    CdlLOC loc;
    u_char result[8];
    u32* var_a1;
    s32 var_t0;
    u32* var_a0;
    u32 var_v1_2;
    u32 var_v1_3;
    u8* var_v1;

    if (D_800B89F4 == 1) {
        return;
    }
    if ((D_800A801C != 0) && (*D_8009B34C & 0x01000000)) {
        D_800B0CD0 = true;
        if (D_800C0DB8 != 0) {
            D_800BCD7C++;
        }
        D_8009B374 = 1;
        return;
    }
    if (CdReady(1, result) == CdlDiskError) {
        return;
    }
    response_words.status = result[0];
    response_words.detail = result[1];
    if (response_words.status & 4) {
        D_8009B374 = 3;
        return;
    }
    D_800A34A0 = (u16 *)&StRingAddr[D_800BE998];
    if (D_800A34A0[0] != 0) {
        if (D_800C0DB8 != 0) {
            D_800BCD7C++;
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
    if (D_800A8020 == 0) {
        var_v1 = (u8 *)&loc;
        do {
            *var_v1++ = *D_8009B334;
        } while (var_v1 < (u8 *)&loc + sizeof(loc));
        for (var_v1_2 = 0; var_v1_2 < 8; var_v1_2++) {
            *D_8009B334;
        }
    }
    var_t0 = 0x11000000;
    if (D_800C0DB8 != 0) {
        mem2mem(D_800A34A0,
                (s32 *)(D_800BCD7C * 0x800 + (u32)D_800C0DB8), 8, 0);
    } else {
        dma_execute(3, D_800A34A0, 0, 8, var_t0, 0, 0);
    }
    while (*D_8009B35C & 0x01000000) {
    }
    ((StHEADER*)D_800A34A0)->loc = loc;
    *D_8009B33C = 0x20843;
    *D_8009B340 = 0x1325;
    if ((g_CdStreamMask == 1) && (D_800B6918 != 0)) {
        if (D_800B6918 != D_800A34A0[4]) {
            D_800A34A0[0] = 0;
            if (D_800C0DB8 != 0) {
                D_800BCD7C++;
            }
            return;
        }
        g_CdStreamMask = 0;
    }
    if ((D_800A34A0[0] != 0x160) ||
        (((CURRENT_STREAM_HEADER->type >> 0xA) & 0x1F) != D_800B8620)) {
        if (D_800C0DB8 != 0) {
            D_800BCD7C = 0;
        } else {
            D_800A34A0[0];
        }
        D_8009B374 = 5;
        D_800A34A0[0] = 0;
        return;
    }
    if ((D_800A8018 != CURRENT_STREAM_HEADER->secCount) ||
        ((D_800A5D54 != 0) && (D_800A5D54 != D_800A34A0[4]))) {
        D_800A5D54 = 0;
        D_800A8018 = 0;
        init_ring_status(D_800BE9E4, D_800BE998 - D_800BE9E4);
        D_800BE998 = D_800BE9E4;
        D_800A34A0[0] = 0;
        if (D_800C0DB8 != 0) {
            D_800BCD7C++;
        }
        D_8009B374 = 6;
        return;
    }
    if (CURRENT_STREAM_HEADER->secCount == 0) {
        D_800A8018 = 0;
        D_800A5D54 = D_800A34A0[4];
        if ((g_CdStreamEndSector != 0) &&
            (D_800A5D54 >= g_CdStreamEndSector)) {
            D_800A5D54 = 0;
            D_800A8018 = 0;
            init_ring_status(D_800BE9E4, D_800BE998 - D_800BE9E4);
            D_800BE998 = D_800BE9E4;
            D_800A34A0[0] = 0;
            g_CdStreamMask = 1;
            if (D_800B0CCC != NULL) {
                D_800B0CCC();
            }
            if (D_800C0DB8 != 0) {
                D_800BCD7C++;
            }
            D_8009B374 = 7;
            return;
        }
        if ((u32)(StRingSize - D_800BE998 - 1) <
            CURRENT_STREAM_HEADER->nSectors) {
            if (g_CdStreamEndSector == 0) {
                D_800A34A0[0] = 1;
                g_CdStreamMask = 1;
                if (D_800B0CCC != NULL) {
                    D_800B0CCC();
                }
                if (D_800C0DB8 != 0) {
                    D_800BCD7C++;
                }
                D_8009B374 = 8;
                return;
            }
            if ((s16)StRingAddr->id != 0) {
                D_800A34A0[0] = 0;
                if (D_800C0DB8 != 0) {
                    D_800BCD7C++;
                }
                D_8009B374 = 9;
                return;
            }
            D_800A34A0[0] = 1;
            var_a1 = StRingAddr;
            var_a0 = D_800A34A0;
            D_800BE998 = 0;
            for (var_v1_3 = 0; var_v1_3 < 8; var_v1_3++) {
                *var_a1++ = *var_a0++;
            }
            D_800A34A0 = (u16 *)StRingAddr;
        }
        D_800BE9E4 = D_800BE998;
    }
    D_8009B374 = 10;
    D_800A8018++;
    D_800C0DC4 =
        (u8 *)&StRingAddr[StRingSize] +
        D_800BE998 * STREAM_SECTOR_PAYLOAD_SIZE;

    if (D_800A801C != 0) {
        var_t0 = 0x11000000;
        *D_8009B33C = 0x20943;
        *D_8009B340 = 0x1323;
    } else {
        *D_8009B33C = 0x21020843;
        var_t0 = 0x11400100;
    }
    if ((CURRENT_STREAM_HEADER->nSectors - 1) ==
        CURRENT_STREAM_HEADER->secCount) {
        D_800B89F4 = 1;
        if (D_800C0DB8 != 0) {
            mem2mem(
                (s32 *)D_800C0DC4,
                (s32 *)(D_800BCD7C * 0x800 + (u32)D_800C0DB8 + 0x20),
                0x1F8, 1);
            D_800BCD7C++;
        } else {
            dma_execute(3, D_800C0DC4, 0, 0x1F8, var_t0, 1, 0);
        }
        D_800A8018 = 0;
        D_800A5D54 = 0;
        D_800B8620 = D_800B6914;
    } else {
        if (D_800C0DB8 != 0) {
            mem2mem(
                (s32 *)D_800C0DC4,
                (s32 *)(D_800BCD7C * 0x800 + (u32)D_800C0DB8 + 0x20),
                0x1F8, 0);
            D_800BCD7C++;
        } else {
            dma_execute(3, D_800C0DC4, 0, 0x1F8, var_t0, 0, 0);
        }
    }
    *D_8009B340 = 0x1325;
    D_800A34A0[0] = 3;
    D_800BE998 += 1;
    if ((D_800C0DB8 != 0) && (D_800B89F4 != 0)) {
        data_ready_callback();
    }
}

void mem2mem(s32* dst, s32* src, u32 num, u32 arg3) {
    u32 i;
    for (i = 0; i < num; i++) {
        *dst++ = *src++;
    }
}
