/* MASPSX_FLAGS: --expand-div */

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

extern char *g_AkaoCurTrack;
extern int g_AkaoInstrumentTable;
extern int g_AkaoPitchPeriodTable;
extern unsigned int g_SpuActiveVoiceMask;

void SeqOp_SetVoiceInstrument(void *track, int arg1, int arg2);

void Akao_PlayNote(void *ptr, unsigned int mask) {
    u8 *pc;
    unsigned int note;
    unsigned int old_note;
    int table;

    pc = *(u8 **)ptr;
    *(u8 **)ptr = pc + 1;
    note = *pc;

    if (*(u16 *)((char *)ptr + 0x54) == 0) {
        if ((*(u32 *)g_AkaoCurTrack & 0x100) != 0) {
            if (note >= 0x20) {
                note += 0x30;
            }
        }
    }

    table = (int)&g_AkaoInstrumentTable + (note << 6);
    old_note = *(u16 *)((char *)ptr + 0x5A);
    *(u16 *)((char *)ptr + 0x5A) = note;
    if (old_note != 0xFF) {
        if (*(u16 *)((char *)ptr + 0x54) == 0) {
            if ((*(u32 *)(g_AkaoCurTrack + 0x14) & mask & g_SpuActiveVoiceMask) != 0) {
                goto call;
            }
        }

        *(u32 *)((char *)ptr + 0xF4) |= 0x10;
        *(int *)((char *)ptr + 0x30) =
            (unsigned int)(*(int *)((char *)ptr + 0x30) * *(int *)(table + 0x10)) /
            *(unsigned int *)((int)&g_AkaoPitchPeriodTable + (old_note << 6));
    }

call:
    SeqOp_SetVoiceInstrument(ptr, table, *(int *)table);
    *(u32 *)((char *)ptr + 0x38) &= ~0x1000;
}

void Akao_TieNote(void *ptr, unsigned int mask) {
    u8 *pc;
    int note;
    unsigned int old_note;
    int table;

    pc = *(u8 **)ptr;
    *(u8 **)ptr = pc + 1;
    note = *pc;

    if (*(u16 *)((char *)ptr + 0x54) == 0) {
        if ((*(u32 *)g_AkaoCurTrack & 0x100) != 0) {
            if (note >= 0x20) {
                note += 0x30;
            }
        }
    }

    table = (int)&g_AkaoInstrumentTable + (note << 6);
    old_note = *(u16 *)((char *)ptr + 0x5A);
    *(u16 *)((char *)ptr + 0x5A) = note;
    if (old_note != 0xFF) {
        if (*(u16 *)((char *)ptr + 0x54) == 0) {
            if ((*(u32 *)(g_AkaoCurTrack + 0x14) & mask & g_SpuActiveVoiceMask) != 0) {
                goto call;
            }
        }

        *(u32 *)((char *)ptr + 0xF4) |= 0x10;
        *(int *)((char *)ptr + 0x30) =
            (unsigned int)(*(int *)((char *)ptr + 0x30) * *(int *)(table + 0x10)) /
            *(unsigned int *)((int)&g_AkaoPitchPeriodTable + (old_note << 6));
    }

call:
    SeqOp_SetVoiceInstrument(ptr, table, 0x1010);
    *(u32 *)((char *)ptr + 0x38) &= ~0x1000;
}
