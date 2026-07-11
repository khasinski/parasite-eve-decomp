typedef signed short s16;
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

extern int D_8009B374;
extern int D_8009B34C;
extern int D_8009B32C;
extern int D_8009B334;
extern int D_8009B338;
extern int D_8009B33C;
extern int D_8009B340;
extern int D_8009B35C;
extern int D_800A34A0;
extern s16 D_800A8018;
extern int D_800A801C;
extern int D_800A8020;
extern int D_800A5D54;
extern int D_800B0CCC;
extern int D_800B0CD0;
extern int D_800B6914;
extern int D_800B6918;
extern int D_800B8620;
extern int D_800B89F4;
extern int D_800BCD7C;
extern int D_800BE998;
extern int D_800BE9E4;
extern int D_800C0DB8;
extern int D_800C0DBC;
extern int D_800C0DC0;
extern int D_800C0DC4;
extern int D_800C0DC8;
extern int D_800C20C4;

int CdReady(int arg0, void *status);
void data_ready_callback(void);
void dma_execute(int chan, u32 madr, u32 bcr_hi, u32 bcr_lo, u32 chcr, u8 enable);
void init_ring_status(int start, u32 count);
void mem2mem(int *dst, int *src, unsigned int count);

#define U8P(addr) ((u8 *)(addr))
#define U16P(addr) ((u16 *)(addr))
#define S16P(addr) ((s16 *)(addr))
#define U32P(addr) ((u32 *)(addr))

static void Render_SetStage(int stage) {
    if (D_800C0DB8 != 0) {
        D_800BCD7C++;
    }
    D_8009B374 = stage;
}

static void Render_ResetRingStatus(int stage) {
    init_ring_status(D_800BE9E4, D_800BE998 - D_800BE9E4);
    D_800A5D54 = 0;
    D_800A8018 = 0;
    D_800BE998 = D_800BE9E4;
    *U16P(D_800A34A0) = 0;
    Render_SetStage(stage);
}

static void Render_CallReadyHook(void) {
    void (*callback)(void);

    callback = (void (*)(void))D_800B0CCC;
    D_800C0DC0 = 1;
    if (callback != 0) {
        callback();
    }
}

static void Render_CopyToCdBuffer(int *src, unsigned int count, int enable) {
    if (D_800C0DB8 != 0) {
        mem2mem(src, (int *)(D_800C0DB8 + (D_800BCD7C << 11) + 0x20), count);
        D_800BCD7C++;
    } else {
        dma_execute(3, (u32)src, 0, count, D_800A801C != 0 ? 0x11000000 : 0x11400100, enable);
    }
}

void Render_InitParticlePool(void) {
    u8 status[4];
    u8 dma_header[4];
    u8 *entry;
    u8 *reg;
    int slot;
    int header_id;
    int next;
    int value;
    u32 dma_chcr;

    if (D_800B89F4 == 1) {
        return;
    }

    if (D_800A801C != 0 && (*U32P(D_8009B34C) & 0x01000000)) {
        D_800B0CD0 = 1;
        Render_SetStage(1);
        return;
    }

    if (CdReady(1, status) == 5) {
        return;
    }

    if (status[0] & 4) {
        D_8009B374 = 3;
        return;
    }

    D_800A34A0 = D_800C0DC8 + (D_800BE998 << 5);
    entry = U8P(D_800A34A0);
    if (*U16P(entry) != 0) {
        Render_SetStage(4);
        return;
    }

    *U8P(D_8009B32C) = 0;
    *U8P(D_8009B338) = 0;
    *U8P(D_8009B32C) = 0;
    *U8P(D_8009B338) = 0x80;
    *U32P(D_8009B33C) = 0x20943;
    *U32P(D_8009B340) = 0x1323;

    if (D_800A8020 == 0) {
        dma_header[0] = *U8P(D_8009B334);
        dma_header[1] = *U8P(D_8009B334);
        dma_header[2] = *U8P(D_8009B334);
        dma_header[3] = *U8P(D_8009B334);
    }

    if (D_800C0DB8 != 0) {
        mem2mem((int *)D_800A34A0, (int *)(D_800C0DB8 + (D_800BCD7C << 11)), 8);
    } else {
        dma_execute(3, D_800A34A0, 0, 8, 0x11000000, 0);
    }

    reg = U8P(D_8009B35C);
    while (*U32P(reg) & 0x01000000) {
    }

    *U32P(D_8009B33C) = 0x20843;
    *U32P(D_8009B340) = 0x1325;
    *U32P(D_800A34A0 + 0x1C) = *(u32 *)dma_header;

    if (D_800C0DC0 == 1 && D_800B6918 != 0) {
        if (D_800B6918 != *U16P(D_800A34A0 + 8)) {
            *U16P(D_800A34A0) = 0;
            if (D_800C0DB8 != 0) {
                D_800BCD7C++;
            }
            return;
        }
        D_800C0DC0 = 0;
    }

    entry = U8P(D_800A34A0);
    if (*U16P(entry) != 0x160 || ((*U16P(entry + 2) >> 10) & 0x1F) != D_800B8620) {
        if (D_800C0DB8 != 0) {
            D_800BCD7C = 0;
        }
        *U16P(D_800A34A0) = 0;
        D_8009B374 = 5;
        return;
    }

    if (D_800A8018 != *U16P(entry + 4)) {
        Render_ResetRingStatus(6);
        return;
    }

    if (D_800A5D54 != 0 && D_800A5D54 != *U16P(entry + 8)) {
        Render_ResetRingStatus(6);
        return;
    }

    if (*U16P(entry + 4) != 0) {
        D_8009B374 = 10;
    } else {
        D_800A8018 = 0;
        D_800A5D54 = *U16P(entry + 8);
        if (D_800C0DBC != 0 && (u32)D_800A5D54 >= (u32)D_800C0DBC) {
            init_ring_status(D_800BE9E4, D_800BE998 - D_800BE9E4);
            D_800A5D54 = 0;
            D_800A8018 = 0;
            D_800BE998 = D_800BE9E4;
            *U16P(D_800A34A0) = 0;
            Render_CallReadyHook();
            Render_SetStage(7);
            return;
        }

        next = (D_800C20C4 - D_800BE998) - 1;
        if ((u32)next < *U16P(entry + 6)) {
            if (D_800C0DBC == 0) {
                *U16P(entry) = 1;
                Render_CallReadyHook();
                Render_SetStage(8);
                return;
            }

            if (*S16P(D_800C0DC8) != 0) {
                *U16P(entry) = 0;
                Render_SetStage(9);
                return;
            }

            *U16P(entry) = 1;
            D_800BE998 = 0;
            entry = U8P(D_800A34A0);
            reg = U8P(D_800C0DC8);
            for (slot = 0; slot < 8; slot++) {
                *U32P(reg) = *U32P(entry);
                entry += 4;
                reg += 4;
            }
            D_800A34A0 = D_800C0DC8;
        }

        D_800BE9E4 = D_800BE998;
        D_8009B374 = 10;
    }

    D_800A8018++;
    D_800C0DC4 = D_800C0DC8 + (D_800C20C4 << 5) + (((D_800BE998 << 6) - D_800BE998) << 5);

    if (D_800A801C != 0) {
        *U32P(D_8009B33C) = 0x20943;
        *U32P(D_8009B340) = 0x1323;
        dma_chcr = 0x11000000;
    } else {
        *U32P(D_8009B33C) = 0x21020843;
        dma_chcr = 0x11400100;
    }

    header_id = *U16P(D_800A34A0 + 6) - 1;
    value = *U16P(D_800A34A0 + 4);
    if (header_id == value) {
        D_800B89F4 = 1;
        Render_CopyToCdBuffer((int *)D_800C0DC4, 0x1F8, 1);
        D_800A8018 = 0;
        D_800A5D54 = 0;
        D_800B8620 = D_800B6914;
    } else {
        if (D_800C0DB8 != 0) {
            mem2mem((int *)D_800C0DC4, (int *)(D_800C0DB8 + (D_800BCD7C << 11) + 0x20), 0x1F8);
            D_800BCD7C++;
        } else {
            dma_execute(3, D_800C0DC4, 0, 0x1F8, dma_chcr, 0);
        }
    }

    *U32P(D_8009B340) = 0x1325;
    *U16P(D_800A34A0) = 3;
    D_800BE998++;
    if (D_800C0DB8 != 0 && D_800B89F4 != 0) {
        data_ready_callback();
    }
}
