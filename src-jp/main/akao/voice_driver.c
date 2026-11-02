#include "common.h"
#include "pe1/akao.h"
#include "pe1/akao/commands.h"
#include "pe1/akao/globals.h"
#include "pe1/akao/spu_registers.h"
#include "pe1/akao/tick.h"
#include "pe1/akao/track.h"
#include "pe1/akao/voice_masks.h"
#include "pe1/akao/voice_state.h"
#include "pe1/psyq_cd.h"
#include "pe1/psyq_spu_internal.h"

int Akao_Cmd_10(void *bank) {
    g_AkaoCmd.opcode = 0x10;
    g_AkaoCmd.arg0.sample_header = bank;
    return Akao_EnqueueStagedCommand();
}

void Akao_Cmd_11(int arg0) {
    g_AkaoCmd.opcode = 0x11;
    g_AkaoCmd.arg0.value = arg0;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_40(void) {
    g_AkaoCmd.opcode = 0x40;
    Akao_EnqueueStagedCommand();
}

int Akao_Cmd_19_Then_C0(int arg0, int arg1) {
    int *opcode;
    int saved_arg;
    int next_opcode;
    int ret;

    opcode = &g_AkaoCmd.opcode;
    *opcode = 0x19;
    g_AkaoCmd.arg0.value = arg0;
    saved_arg = arg1;
    ret = Akao_EnqueueStagedCommand();

    next_opcode = 0xC0;
    *opcode = next_opcode;
    saved_arg &= 0x7F;
    g_AkaoCmd.arg0.value = saved_arg;
    g_AkaoCmd.arg3 = 0;
    saved_arg = ret;
    Akao_EnqueueStagedCommand();

    return saved_arg;
}

void Akao_Cmd_12(int arg0, int arg1) {
    g_AkaoCmd.opcode = 0x12;
    g_AkaoCmd.arg0.value = arg0;
    g_AkaoCmd.arg1 = arg1;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_20(int arg0, int arg1, int arg2, int arg3) {
    g_AkaoCmd.opcode = 0x20;
    g_AkaoCmd.arg0.value = arg0 & 0x3FF;
    g_AkaoCmd.arg1 = arg1 & 0xFFFFFF;
    g_AkaoCmd.arg2 = arg2 & 0xFF;
    g_AkaoCmd.arg3 = arg3 & 0x7F;
    Akao_EnqueueStagedCommand();
}

int Akao_Cmd_24(void *header, int arg1, int arg2, int arg3) {
    int status = Spu_ValidateSampleHeader(header);
    if (status != 0) {
        return status;
    }

    g_AkaoCmd.opcode = 0x24;
    g_AkaoCmd.arg0.command_data = ((AkaoFilePrefix *)header)->contents;
    g_AkaoCmd.arg1 = arg1 & 0xFFFFFF;
    g_AkaoCmd.arg2 = arg2 & 0xFF;
    g_AkaoCmd.arg3 = arg3 & 0x7F;
    return Akao_EnqueueStagedCommand();
}

void Akao_Cmd_21(int arg0, int arg1) {
    g_AkaoCmd.opcode = 0x21;
    g_AkaoCmd.arg0.value = arg0 & 0xFFFF;
    g_AkaoCmd.arg1 = arg1 & 0xFFFFFF;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_30(int arg0) {
    g_AkaoCmd.opcode = 0x30;
    g_AkaoCmd.arg0.value = arg0 & 0x3FF;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_80_81_82(int arg0) {
    int opcode;

    switch (arg0) {
    case 1:
        opcode = 0x81;
        break;
    case 2:
        opcode = 0x82;
        break;
    default:
        opcode = 0x80;
        break;
    }

    g_AkaoCmd.opcode = opcode;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_90(int arg0) {
    g_AkaoCmd.opcode = 0x90;
    g_AkaoCmd.arg0.value = arg0 & 0xFFFFFF;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_92(int arg0) {
    g_AkaoCmd.opcode = 0x92;
    g_AkaoCmd.arg0.value = arg0;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_99_9B_9D(int arg0) {
    int opcode;

    switch (arg0) {
    case 1:
        opcode = 0x9B;
        break;
    case 2:
        opcode = 0x9D;
        break;
    default:
        opcode = 0x99;
        break;
    }

    g_AkaoCmd.opcode = opcode;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_98_9A_9C(int arg0) {
    int opcode;

    switch (arg0) {
    case 1:
        opcode = 0x9A;
        break;
    case 2:
        opcode = 0x9C;
        break;
    default:
        opcode = 0x98;
        break;
    }

    g_AkaoCmd.opcode = opcode;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_A8(int arg0) {
    g_AkaoCmd.opcode = 0xA8;
    g_AkaoCmd.arg0.value = arg0 & 0x7F;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_A9(int arg0, int arg1) {
    g_AkaoCmd.opcode = 0xA9;
    g_AkaoCmd.arg0.value = arg0 & 0xFF;
    g_AkaoCmd.arg1 = arg1 & 0x7F;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_A0(int arg0, int arg1, int arg2) {
    g_AkaoCmd.opcode = 0xA0;
    g_AkaoCmd.arg0.value = arg0 & 0xFFFF;
    g_AkaoCmd.arg1 = arg1 & 0xFFFFFF;
    g_AkaoCmd.arg2 = arg2 & 0x7F;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_A1(int arg0, int arg1, int arg2, int arg3) {
    g_AkaoCmd.opcode = 0xA1;
    g_AkaoCmd.arg0.value = arg0 & 0xFFFF;
    g_AkaoCmd.arg1 = arg1 & 0xFFFFFF;
    g_AkaoCmd.arg2 = arg2 & 0xFF;
    g_AkaoCmd.arg3 = arg3 & 0x7F;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_AA(int arg0) {
    g_AkaoCmd.opcode = 0xAA;
    g_AkaoCmd.arg0.value = arg0 & 0xFF;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_AB(int arg0, int arg1) {
    g_AkaoCmd.opcode = 0xAB;
    g_AkaoCmd.arg0.value = arg0 & 0xFF;
    g_AkaoCmd.arg1 = arg1 & 0xFF;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_A2(int arg0, int arg1, int arg2) {
    g_AkaoCmd.opcode = 0xA2;
    g_AkaoCmd.arg0.value = arg0 & 0x3FF;
    g_AkaoCmd.arg1 = arg1 & 0xFFFFFF;
    g_AkaoCmd.arg2 = arg2 & 0xFF;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_A3(int arg0, int arg1, int arg2, int arg3) {
    g_AkaoCmd.opcode = 0xA3;
    g_AkaoCmd.arg0.value = arg0 & 0x3FF;
    g_AkaoCmd.arg1 = arg1 & 0xFFFFFF;
    g_AkaoCmd.arg2 = arg2 & 0xFF;
    g_AkaoCmd.arg3 = arg3 & 0xFF;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_AC(int arg0) {
    g_AkaoCmd.opcode = 0xAC;
    g_AkaoCmd.arg0.value = arg0 & 0xFF;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_AD(int arg0, int arg1) {
    g_AkaoCmd.opcode = 0xAD;
    g_AkaoCmd.arg0.value = arg0 & 0xFF;
    g_AkaoCmd.arg1 = arg1 & 0xFF;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_A4(int arg0, int arg1, int arg2) {
    g_AkaoCmd.opcode = 0xA4;
    g_AkaoCmd.arg0.value = arg0 & 0xFFFF;
    g_AkaoCmd.arg1 = arg1 & 0xFFFFFF;
    g_AkaoCmd.arg2 = arg2 & 0xFF;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_A5(int arg0, int arg1, int arg2, int arg3) {
    g_AkaoCmd.opcode = 0xA5;
    g_AkaoCmd.arg0.value = arg0 & 0xFFFF;
    g_AkaoCmd.arg1 = arg1 & 0xFFFFFF;
    g_AkaoCmd.arg2 = arg2 & 0xFF;
    g_AkaoCmd.arg3 = arg3 & 0xFF;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_C0_WithSlot(int arg0, int arg1) {
    g_AkaoCmd.opcode = 0xC0;
    g_AkaoCmd.arg0.value = arg1 & 0x7F;
    g_AkaoCmd.arg3 = arg0;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_C1_WithSlot(int arg0, int arg1, int arg2) {
    g_AkaoCmd.opcode = 0xC1;
    g_AkaoCmd.arg0.value = arg1;
    g_AkaoCmd.arg1 = arg2 & 0x7F;
    g_AkaoCmd.arg3 = arg0;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_C2_WithSlot(int arg0, int arg1, int arg2, int arg3) {
    g_AkaoCmd.opcode = 0xC2;
    g_AkaoCmd.arg0.value = arg1;
    g_AkaoCmd.arg1 = arg2 & 0x7F;
    g_AkaoCmd.arg2 = arg3 & 0x7F;
    g_AkaoCmd.arg3 = arg0;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_C8(int arg0) {
    g_AkaoCmd.opcode = 0xC8;
    g_AkaoCmd.arg0.value = arg0;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_C9(int arg0, int arg1) {
    g_AkaoCmd.opcode = 0xC9;
    g_AkaoCmd.arg0.value = arg0;
    g_AkaoCmd.arg1 = arg1;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_CA(int arg0, int arg1, int arg2) {
    g_AkaoCmd.opcode = 0xCA;
    g_AkaoCmd.arg0.value = arg0;
    g_AkaoCmd.arg1 = arg1;
    g_AkaoCmd.arg2 = arg2;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_D0(int arg0) {
    g_AkaoCmd.opcode = 0xD0;
    g_AkaoCmd.arg0.value = arg0 & 0xFF;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_D1(int arg0, int arg1) {
    g_AkaoCmd.opcode = 0xD1;
    g_AkaoCmd.arg0.value = arg0;
    g_AkaoCmd.arg1 = arg1 & 0xFF;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_D2(int arg0, int arg1, int arg2) {
    g_AkaoCmd.opcode = 0xD2;
    g_AkaoCmd.arg0.value = arg0;
    g_AkaoCmd.arg1 = arg1 & 0xFF;
    g_AkaoCmd.arg2 = arg2 & 0xFF;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_D4(int arg0) {
    g_AkaoCmd.opcode = 0xD4;
    g_AkaoCmd.arg0.value = arg0 & 0xFF;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_D5(int arg0, int arg1) {
    g_AkaoCmd.opcode = 0xD5;
    g_AkaoCmd.arg0.value = arg0;
    g_AkaoCmd.arg1 = arg1 & 0xFF;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_D6(int arg0, int arg1, int arg2) {
    g_AkaoCmd.opcode = 0xD6;
    g_AkaoCmd.arg0.value = arg0;
    g_AkaoCmd.arg1 = arg1 & 0xFF;
    g_AkaoCmd.arg2 = arg2 & 0xFF;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_D8(int arg0) {
    g_AkaoCmd.opcode = 0xD8;
    g_AkaoCmd.arg0.value = arg0 & 0xFF;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_D9(int arg0, int arg1) {
    g_AkaoCmd.opcode = 0xD9;
    g_AkaoCmd.arg0.value = arg0;
    g_AkaoCmd.arg1 = arg1 & 0xFF;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_DA(int arg0, int arg1, int arg2) {
    g_AkaoCmd.opcode = 0xDA;
    g_AkaoCmd.arg0.value = arg0;
    g_AkaoCmd.arg1 = arg1 & 0xFF;
    g_AkaoCmd.arg2 = arg2 & 0xFF;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_F0(void) {
    g_AkaoCmd.opcode = 0xF0;
    Akao_EnqueueStagedCommand();
}

void Akao_Cmd_F1(void) {
    g_AkaoCmd.opcode = 0xF1;
    Akao_EnqueueStagedCommand();
}

int Spu_PollOrWaitSeqBusy(int mode) {
    if (mode == 0) {
        while (g_AkaoSeqBusyFlags & 1) {
        }
        return 0;
    }

    return g_AkaoSeqBusyFlags & 1;
}

int Spu_UploadSampleBlockBlocking(void *arg0, int arg1) {
    int result;

    while ((result = Spu_UploadSampleBlock(arg0, arg1)) == 1) {
    }
    return result;
}

int Spu_GetTransferStatus(void) {
    return g_SpuTransferStatus;
}

static inline unsigned char ScaleVolume(int volume) {
    unsigned int value = volume;

    value <<= 1;
    value += volume;
    value <<= 2;
    value -= volume;
    value += value << 5;
    value <<= 3;
    value -= volume;
    return value >> 13;
}

void Spu_SetVoiceVolume(int arg0) {
    unsigned char value;

    if (g_AkaoPlaybackMode & 2) {
        value = ScaleVolume(arg0);
        g_AkaoCdVolume.val3 = value;
        g_AkaoCdVolume.val1 = value;
        g_AkaoCdVolume.val2 = value;
        g_AkaoCdVolume.val0 = value;
    } else {
        value = arg0;
        g_AkaoCdVolume.val2 = value;
        g_AkaoCdVolume.val0 = value;
        g_AkaoCdVolume.val3 = 0;
        g_AkaoCdVolume.val1 = 0;
    }
    CdMix(&g_AkaoCdVolume);
}

int Spu_SetStreamModeA(void) {
    g_AkaoStreamUploadMode = 1;
    return 0;
}

int Akao_StepNoteSequencer(unsigned char *input, unsigned available)
{
    unsigned char *instruments;
    unsigned payload_size;
    unsigned count;
    unsigned address;
    unsigned address_offset;
    unsigned upload_size;
    unsigned after_header;
    unsigned char *rebase_instruments;
    unsigned rebase_address;
    unsigned next_address;
    unsigned remaining_payload;
    unsigned start;
    unsigned *dest;
    AkaoInstrumentAddress cache;
    unsigned result;

    if (g_AkaoStreamUploadMode & 1) {
        if (Spu_ValidateSampleHeader(input)) return -1;
        input += 0x14;
        asm volatile("" : "=r"(input) : "0"(input));
        payload_size = *(unsigned *)input;
        input += 4;
        asm volatile("" : "=r"(input) : "0"(input));
        start = *(unsigned *)input;
        input += 4;
        asm volatile("" : "=r"(input) : "0"(input));
        count = *(unsigned *)input;
        if (!count) count = 256;
        count -= start;
        input += 0x24;
        instruments = input;
        upload_size = count << 6;
        input += upload_size;
        after_header = available - 0x40;
        available = after_header - upload_size;
        asm volatile("" : "=r"(available) : "0"(available));
        upload_size = available;
        if (upload_size >= payload_size) upload_size = payload_size;

        if (count < 0x31) {
            if (g_AkaoCurTrack[1].active_voice_mask != 0) {
                if ((g_AkaoCurTrack[1].status_flags & 0x100) == 0) {
                    address = 0x38000;
                    g_AkaoVoiceKeyOnState |= 0x100;
                } else {
                    address = 0x8000;
                    g_AkaoVoiceKeyOnState &= ~0x100U;
                }
            } else {
                address = 0x8000;
                if (g_AkaoCurTrack[0].active_voice_mask != 0) {
                    if ((g_AkaoCurTrack[0].status_flags & 0x100) == 0) {
                        address = 0x38000;
                        g_AkaoVoiceKeyOnState |= 0x100;
                    } else {
                        g_AkaoVoiceKeyOnState &= ~0x100U;
                    }
                } else {
                    g_AkaoVoiceKeyOnState &= ~0x100U;
                }
            }
        } else {
            address = 0x8000;
            g_AkaoVoiceKeyOnState &= ~0x100U;
        }
        SpuSetTransferStartAddr(address);
        Spu_UploadWithPrepare(input, upload_size);
        rebase_instruments = instruments;
        rebase_address = address;
        next_address = address + upload_size;
        g_AkaoInstrumentUploadAddress = next_address;
        remaining_payload = payload_size - upload_size;
        g_AkaoInstrumentUploadRemaining = remaining_payload;
        Spu_RebaseStreamAddrs(rebase_instruments, rebase_address, count);
        address_offset = address == 0x8000 ? 0x20 : 0x50;
        address_offset <<= 6;
        cache.instrument = g_AkaoInstrumentTable;
        cache.word = address_offset + cache.word;
        dest = cache.words;
        upload_size = count << 4;
        do {
            --upload_size;
            *dest++ = *(unsigned *)instruments;
            instruments += 4;
        } while (upload_size);
        g_AkaoStreamUploadMode &= ~1U;
        return g_AkaoInstrumentUploadRemaining;
    }
    SpuSetTransferStartAddr(g_AkaoInstrumentUploadAddress);
    upload_size = available;
    if (upload_size >= g_AkaoInstrumentUploadRemaining) upload_size = g_AkaoInstrumentUploadRemaining;
    Spu_UploadWithPrepare(input, upload_size);
    g_AkaoInstrumentUploadAddress += upload_size;
    g_AkaoInstrumentUploadRemaining -= upload_size;
    return g_AkaoInstrumentUploadRemaining;
}

int Spu_SetStreamModeB(void) {
    g_AkaoStreamUploadMode = 2;
    return 0;
}

int Spu_UploadStreamBlockA(unsigned bank, unsigned char *input, unsigned available)
{
    unsigned char *instruments;
    unsigned payload_size;
    unsigned count;
    unsigned address;
    unsigned upload_size;
    unsigned start;
    unsigned after_header;
    unsigned *dest;
    AkaoInstrumentAddress cache;

    if (g_AkaoStreamUploadMode & 2) {
        if (Spu_ValidateSampleHeader(input)) return -1;
        input += 0x14;
        asm volatile("" : "=r"(input) : "0"(input));
        payload_size = *(unsigned *)input;
        input += 4;
        asm volatile("" : "=r"(input) : "0"(input));
        start = *(unsigned *)input;
        input += 4;
        asm volatile("" : "=r"(input) : "0"(input));
        count = *(unsigned *)input;
        if (!count) count = 256;
        count -= start;
        input += 0x24;
        instruments = input;
        upload_size = count << 6;
        input += upload_size;
        after_header = available - 0x40;
        available = after_header - upload_size;
        asm volatile("" : "=r"(available) : "0"(available));
        upload_size = available;
        if (upload_size >= payload_size) upload_size = payload_size;
        address = ((bank * 5) << 13) + 0x4f000;
        SpuSetTransferStartAddr(address);
        Spu_UploadWithPrepare(input, upload_size);
        g_AkaoStreamAAddress = address + upload_size;
        g_AkaoStreamARemaining = payload_size - upload_size;
        Spu_RebaseStreamAddrs(instruments, address, count);
        address = bank << 10;
        cache.instrument = &g_AkaoStreamInstrumentCacheB[16];
        cache.word = address + cache.word;
        dest = cache.words;
        upload_size = count << 4;
        do {
            --upload_size;
            *dest++ = *(unsigned *)instruments;
            instruments += 4;
        } while (upload_size);
        g_AkaoStreamUploadMode &= ~2U;
        return g_AkaoStreamARemaining;
    }
    SpuSetTransferStartAddr(g_AkaoStreamAAddress);
    upload_size = available;
    if (upload_size >= g_AkaoStreamARemaining) upload_size = g_AkaoStreamARemaining;
    Spu_UploadWithPrepare(input, upload_size);
    g_AkaoStreamAAddress += upload_size;
    g_AkaoStreamARemaining -= upload_size;
    return g_AkaoStreamARemaining;
}

int Spu_UploadStreamBlockB(unsigned bank, unsigned char *cursor)
{
    unsigned char *instruments;
    unsigned size, start, count, address;
    AkaoInstrumentAddress cache;
    unsigned *dest;
    if (bank & ~1U) return 1;
    if (Spu_ValidateSampleHeader(cursor)) return -1;
    /* Header words: payload size, first instrument, exclusive end.
     * An encoded end of zero denotes instrument 256. */
    cursor += 0x14;
    size = *(unsigned *)cursor;
    cursor += 4;
    start = *(unsigned *)cursor;
    cursor += 4;
    count = *(unsigned *)cursor;
    if (!count) count = 256;
    count -= start;
    /* The 0x40-byte header is followed by 0x40-byte instrument records. */
    instruments = cursor + 0x24;
    cursor = instruments + (count << 6);
    address = 0x68000 + (bank << 13);
    SpuSetTransferStartAddr(address);
    Spu_UploadWithPrepare(cursor, size);
    Spu_RebaseStreamAddrs(instruments,address,count);
    /* Cache the relocated records after the SPU payload upload. */
    address = bank << 10;
    cache.instrument = g_AkaoStreamInstrumentCacheB;
    cache.word = address + cache.word;
    dest = cache.words;
    size = count << 4;
    do {
        --size;
        *dest++ = *(unsigned *)instruments;
        instruments += 4;
    } while (size);
    return 0;
}

void Spu_WriteKeyOn(u32 value) {
    g_SpuKeyOnLow = value;
    value >>= 16;
    g_SpuKeyOnHigh = value;
}

void Spu_WriteKeyOff(u32 value) {
    g_SpuKeyOffLow = value;
    value >>= 16;
    g_SpuKeyOffHigh = value;
}

void Spu_WriteReverbEnable(u32 value) {
    g_SpuReverbEnableLow = value;
    value >>= 16;
    g_SpuReverbEnableHigh = value;
}

void Spu_WriteNoiseEnable(u32 value) {
    g_SpuNoiseEnableLow = value;
    value >>= 16;
    g_SpuNoiseEnableHigh = value;
}

void Spu_WriteFmEnable(u32 value) {
    g_SpuPitchModLow = value;
    value >>= 16;
    g_SpuFmEnableHigh = value;
}

void AkaoSpuVoice_SetVolume(u32 index, u32 left, u32 right) {
    u16 *ptr;
    u32 mask;

    ptr = (u16 *)(0x1F801C00 + (index * 0x10));
    mask = 0x7FFF;
    ptr[0] = left & mask;
    ptr++;
    *ptr = right & 0x7FFF;
}

void AkaoSpuVoice_SetPitch(u32 index, u32 value) {
    u16 *ptr;

    ptr = (u16 *)(0x1F800000 + (index * 0x10));
    ptr[0x1C04 / 2] = value;
}

void AkaoSpuVoice_SetStartAddress(u32 index, u32 value) {
    u16 *ptr;

    ptr = (u16 *)(0x1F800000 + (index * 0x10));
    ptr[0x1C06 / 2] = value >> 3;
}

void AkaoSpuVoice_SetRepeatAddress(u32 index, u32 value) {
    u16 *ptr;

    ptr = (u16 *)(0x1F800000 + (index * 0x10));
    ptr[0x1C0E / 2] = value >> 3;
}

void AkaoSpuVoice_SetAdsrAttack(u32 index, u32 left, u32 right) {
    u16 *ptr;
    u32 current;
    u32 value;

    ptr = (u16 *)(0x1F801C08 + (index * 0x10));
    right = (right >> 2) << 15;
    left <<= 8;
    current = *(u8 *)ptr;
    value = right | left;
    *ptr = current | value;
}

void AkaoSpuVoice_SetAdsrDecayRate(u32 index, u32 value) {
    u16 *ptr;
    u32 current;

    ptr = (u16 *)(0x1F801C08 + (index * 0x10));
    current = *ptr;
    value <<= 4;
    current &= 0xFF0F;
    *ptr = current | value;
}

void AkaoSpuVoice_SetAdsrSustainLevel(u32 index, u32 value) {
    u16 *ptr;
    u32 current;

    ptr = (u16 *)(0x1F801C08 + (index * 0x10));
    current = *ptr;
    current &= 0xFFF0;
    *ptr = current | value;
}

void AkaoSpuVoice_SetAdsrSustainRate(u32 index, u32 left, u32 right) {
    u16 *ptr;
    u32 current;
    u32 value;

    ptr = (u16 *)(0x1F801C0A + (index * 0x10));
    right = (right >> 1) << 14;
    left <<= 6;
    current = *ptr;
    value = right | left;
    current &= 0x3F;
    *ptr = current | value;
}

void AkaoSpuVoice_SetAdsrReleaseRate(u32 index, u32 left, u32 right) {
    u16 *ptr;
    u32 current;
    u32 value;

    ptr = (u16 *)(0x1F801C0A + (index * 0x10));
    right = (right >> 2) << 5;
    current = *ptr;
    value = right | left;
    current &= 0xFFC0;
    *ptr = current | value;
}

/* Old-style definition: the stereo and note callers pass the track flags
   (and a mask) after the two parameters read here, so this unit keeps the
   unprototyped call ABI that voice_masks.h declares. */
void Akao_WriteVoiceParam(voice_index, params)
    int voice_index;
    AkaoVoiceParams *params;
{
    unsigned int flags;
    unsigned int cur;

    flags = params->flags;
    if (flags != 0) {
        if (flags & AKAO_VOICE_PARAM_PITCH) {
            AkaoSpuVoice_SetPitch(voice_index, params->pitch);
            params->flags &= ~AKAO_VOICE_PARAM_PITCH;
            if (params->flags == 0) {
                return;
            }
        }
        if (flags & AKAO_VOICE_PARAM_VOLUME) {
            AkaoSpuVoice_SetVolume(voice_index, params->volume_left, params->volume_right);
            params->flags &= ~AKAO_VOICE_PARAM_VOLUME;
            if (params->flags == 0) {
                return;
            }
        }
        if (flags & AKAO_VOICE_PARAM_START_ADDR) {
            AkaoSpuVoice_SetStartAddress(voice_index, params->start_address);
            params->flags &= ~AKAO_VOICE_PARAM_START_ADDR;
            if (params->flags == 0) {
                return;
            }
        }
        if (flags & AKAO_VOICE_PARAM_LOOP_ADDR) {
            AkaoSpuVoice_SetRepeatAddress(voice_index, params->loop_address);
            cur = params->flags & ~AKAO_VOICE_PARAM_LOOP_ADDR;
            params->flags = cur;
            if (cur == 0) {
                return;
            }
        }
        if (flags & AKAO_VOICE_PARAM_ADSR_SUSTAIN) {
            AkaoSpuVoice_SetAdsrSustainRate(voice_index, params->adsr_sustain_rate, params->adsr_sustain_mode);
            params->flags &= ~AKAO_VOICE_PARAM_ADSR_SUSTAIN;
            if (params->flags == 0) {
                return;
            }
        }
        if (flags & AKAO_VOICE_PARAM_ADSR_ATTACK) {
            AkaoSpuVoice_SetAdsrAttack(voice_index, params->adsr_attack_rate, params->adsr_attack_mode);
            params->flags &= ~AKAO_VOICE_PARAM_ADSR_ATTACK;
            if (params->flags == 0) {
                return;
            }
        }
        if (flags & AKAO_VOICE_PARAM_ADSR_RELEASE) {
            AkaoSpuVoice_SetAdsrReleaseRate(voice_index, params->adsr_release_rate, params->adsr_release_mode);
            params->flags &= ~AKAO_VOICE_PARAM_ADSR_RELEASE;
            if (params->flags == 0) {
                return;
            }
        }
        if (flags & AKAO_VOICE_PARAM_ADSR_DECAY_SUSTAIN) {
            AkaoSpuVoice_SetAdsrDecayRate(voice_index, params->adsr_decay_rate);
            AkaoSpuVoice_SetAdsrSustainLevel(voice_index, params->adsr_sustain_level);
        }
        params->flags = 0;
    }
}

void Spu_UpdateVoiceRegisters(AkaoTrack *voice, u32 voice_mask) {
    int new_value;
    int value;
    int scaled;
    s16 *table;

    if (voice->expression_duration != 0) {
        u32 old_value;
        old_value = voice->expression_value;
        new_value = old_value + voice->expression_delta;
        voice->expression_duration--;
        if ((new_value & 0xFFE00000) != (old_value & 0xFFE00000)) {
            voice->update_flags |= 3;
        }
        voice->expression_value = new_value;
    }

    if (voice->pitch_slide_duration != 0) {
        voice->pitch_slide_duration--;
        voice->pitch_slide_current += (u16)voice->pitch_slide_delta;
        voice->update_flags |= 3;
    }

    if (voice->volume_duration != 0) {
        u32 old_value;
        old_value = voice->volume_base;
        new_value = old_value + voice->volume_delta;
        voice->volume_duration--;
        if ((new_value & 0x7F00) != (old_value & 0x7F00)) {
            voice->update_flags |= 3;
        }
        voice->volume_base = new_value;
    }

    if (voice->panpot_duration != 0) {
        u32 signed_old;
        signed_old = voice->pan_target;
        new_value = signed_old + voice->pan_delta;
        voice->panpot_duration--;
        if ((voice->flags & 0x100) != 0 &&
            ((new_value & 0xFF00) != (signed_old & 0xFF00))) {
            voice->update_flags |= 3;
        }
        voice->pan_target = new_value;
    }

    if (voice->panpot_slide_duration != 0) {
        u32 old_value;
        old_value = voice->panpot;
        new_value = old_value + voice->panpot_delta;
        voice->panpot_slide_duration--;
        if ((new_value & 0xFF00) != (old_value & 0xFF00)) {
            voice->update_flags |= 3;
        }
        voice->panpot = new_value;
    }

    if (voice->pitch_lfo_counter != 0) {
        voice->pitch_lfo_counter--;
    }

    if (voice->volume_lfo_counter != 0) {
        voice->volume_lfo_counter--;
    }

    if (voice->key_on_delay != 0) {
        voice->key_on_delay--;
        if (voice->key_on_delay == 0) {
            /* Updated through a plain word pointer: a direct member store
             * would let the scheduler move it past the flag update below. */
            AkaoU32 *mask = &D_8009CD88->bank.key_off_dirty_mask;
            *mask ^= voice_mask;
            g_AkaoVoiceUpdateFlags |= 0x10;
            Seq_MarkTrack34MaskDirty();
        }
    }

    if (voice->key_off_delay != 0) {
        voice->key_off_delay--;
        if (voice->key_off_delay == 0) {
            AkaoU32 *mask = &D_8009CD88->bank.adsr_dirty_mask;
            *mask ^= voice_mask;
            Seq_MarkTrack3CMaskDirty();
        }
    }

    if (voice->pitch_lfo_slide_duration != 0) {
        voice->pitch_lfo_slide_duration--;
        voice->pitch_lfo_target += (u16)voice->pitch_lfo_delta;
        value = (voice->pitch_lfo_target & 0x7F00) >> 8;
        if ((voice->pitch_lfo_target & 0x8000) != 0) {
            scaled = (value * voice->pitch_base) >> 7;
        } else {
            scaled = (value * (((voice->pitch_base << 4) - voice->pitch_base) >> 8)) >> 7;
        }
        voice->pitch_lfo_depth = scaled;

        if (voice->pitch_lfo_counter == 0 && voice->pitch_lfo_phase != 1) {
            table = voice->pitch_lfo_table;
            if (table[0] == 0 && table[1] == 0) {
                table += table[2];
            }
            new_value = (voice->pitch_lfo_depth * table[0]) >> 16;
            if (new_value != voice->pitch_lfo_value) {
                voice->pitch_lfo_value = new_value;
                voice->update_flags |= 0x10;
                if (new_value >= 0) {
                    voice->pitch_lfo_value = new_value << 1;
                }
            }
        }
    }

    if (voice->volume_lfo_slide_duration != 0) {
        voice->volume_lfo_slide_duration--;
        voice->volume_lfo_target += (u16)voice->volume_lfo_delta;
        if (voice->volume_lfo_counter == 0 && voice->volume_lfo_phase != 1) {
            table = voice->volume_lfo_table;
            if (table[0] == 0 && table[1] == 0) {
                table += table[2];
            }
            { int product = (((s16)(voice->expression_value >> 16) * (voice->volume_base >> 8)) >> 7) * (voice->volume_lfo_target >> 8);
              new_value = (product << 9) >> 16;
              new_value = (new_value * table[0]) >> 15; }
            if (new_value != voice->volume_lfo_value) {
                voice->volume_lfo_value = new_value;
                voice->update_flags |= 3;
            }
        }
    }

    if (voice->aux_lfo_slide_duration != 0) {
        voice->aux_lfo_slide_duration--;
        voice->aux_lfo_target += (u16)voice->aux_lfo_delta;
        if (voice->aux_lfo_phase != 1) {
            table = voice->aux_lfo_table;
            if (table[0] == 0 && table[1] == 0) {
                table += table[2];
            }
            new_value = ((voice->aux_lfo_target >> 8) * table[0]) >> 15;
            if (new_value != voice->aux_lfo_value) {
                voice->aux_lfo_value = new_value;
                voice->update_flags |= 3;
            }
        }
    }

    if (voice->pitch_slide_steps != 0) {
        u32 old_value;
        old_value = voice->voice_mask_a;
        new_value = old_value + voice->pitch_slide_step;
        voice->pitch_slide_steps--;
        if ((new_value & 0xFFFF0000) != (old_value & 0xFFFF0000)) {
            voice->update_flags |= 0x10;
        }
        voice->voice_mask_a = new_value;
    }
}

void Spu_TickVoiceEnvelopes(AkaoTrack *track, unsigned voice_mask)
{
    int new_value;
    int value;
    u16 depth;
    s16 *table;

    if (track->expression_duration != 0) {
        u32 old_value;
        old_value = track->expression_value;
        new_value = old_value + track->expression_delta;
        track->expression_duration--;
        if ((new_value & 0xFFE00000) != (old_value & 0xFFE00000)) {
            track->update_flags |= 3;
        }
        track->expression_value = new_value;
    }

    if (track->key_on_delay != 0) {
        track->key_on_delay--;
        if (track->key_on_delay == 0) {
            g_AkaoTrack34Mask ^= voice_mask;
            g_AkaoVoiceUpdateFlags |= 0x10;
            Seq_MarkTrack34MaskDirty();
        }
    }

    if (track->key_off_delay != 0) {
        track->key_off_delay--;
        if (track->key_off_delay == 0) {
            g_AkaoTrack3CMask ^= voice_mask;
            Seq_MarkTrack3CMaskDirty();
        }
    }

    if (track->pitch_lfo_slide_duration != 0) {
        track->pitch_lfo_slide_duration--;
        track->pitch_lfo_target += (u16)track->pitch_lfo_delta;
        value = (track->pitch_lfo_target & 0x7F00) >> 8;
        if ((track->pitch_lfo_target & 0x8000) != 0) {
            depth = (value * track->pitch_base) >> 7;
        } else {
            depth = (value * (((track->pitch_base << 4) - track->pitch_base) >> 8)) >> 7;
        }
        track->pitch_lfo_depth = depth;

        if (track->pitch_lfo_phase != 1) {
            table = track->pitch_lfo_table;
            if (table[0] == 0 && table[1] == 0) {
                table += table[2];
            }
            new_value = (track->pitch_lfo_depth * table[0]) >> 16;
            if (new_value != track->pitch_lfo_value) {
                track->pitch_lfo_value = new_value;
                track->update_flags |= 0x10;
                if (new_value >= 0) {
                    track->pitch_lfo_value = new_value << 1;
                }
            }
        }
    }

    if (track->volume_lfo_slide_duration != 0) {
        track->volume_lfo_slide_duration--;
        track->volume_lfo_target += (u16)track->volume_lfo_delta;
        if (track->volume_lfo_phase != 1) {
            table = track->volume_lfo_table;
            if (table[0] == 0 && table[1] == 0) {
                table += table[2];
            }
            {
                int product = (((s16)(track->expression_value >> 16) * (track->volume_base >> 8)) >> 7) *
                              (track->volume_lfo_target >> 8);
                new_value = (product << 9) >> 16;
                new_value = (new_value * table[0]) >> 15;
            }
            if (new_value != track->volume_lfo_value) {
                track->volume_lfo_value = new_value;
                track->update_flags |= 3;
            }
        }
    }

    if (track->aux_lfo_slide_duration != 0) {
        track->aux_lfo_slide_duration--;
        track->aux_lfo_target += (u16)track->aux_lfo_delta;
        if (track->aux_lfo_phase != 1) {
            table = track->aux_lfo_table;
            if (table[0] == 0 && table[1] == 0) {
                table += table[2];
            }
            new_value = ((track->aux_lfo_target >> 8) * table[0]) >> 15;
            if (new_value != track->aux_lfo_value) {
                track->aux_lfo_value = new_value;
                track->update_flags |= 3;
            }
        }
    }

    if (track->pitch_slide_steps != 0) {
        u32 old_value;
        old_value = track->voice_mask_a;
        new_value = old_value + track->pitch_slide_step;
        track->pitch_slide_steps--;
        if ((new_value & 0xFFFF0000) != (old_value & 0xFFFF0000)) {
            track->update_flags |= 0x10;
        }
        track->voice_mask_a = new_value;
    }
}

void Akao_SetVoiceKeyOff(AkaoTrack *voice, u32 voice_mask, int index) {
    u32 flags;
    int base_volume;
    int value;
    int sample;
    s16 *table;
    AkaoTrack *target;

    base_volume = ((s16)(voice->expression_value >> 16) * (voice->volume_base >> 8)) >> 7;
    flags = voice->flags;

    if ((flags & AKAO_TRACK_FLAG_PITCH_LFO) != 0 && voice->pitch_lfo_counter == 0) {
        if (--voice->pitch_lfo_phase == 0) {
            voice->pitch_lfo_phase = voice->pitch_lfo_duration;
            table = voice->pitch_lfo_table;
            if (table[0] == 0 && table[1] == 0) {
                voice->pitch_lfo_table = table + table[2];
            }
            table = voice->pitch_lfo_table;
            voice->pitch_lfo_table = table + 1;
            sample = *table;
            value = (voice->pitch_lfo_depth * sample) >> 16;
            if (value != voice->pitch_lfo_value) {
                voice->pitch_lfo_value = value;
                voice->update_flags |= AKAO_VOICE_PARAM_PITCH;
                if (value >= 0) {
                    voice->pitch_lfo_value = value << 1;
                }
            }
        }
    }

    if ((flags & AKAO_TRACK_FLAG_VOLUME_LFO) != 0 && voice->volume_lfo_counter == 0) {
        if (--voice->volume_lfo_phase == 0) {
            int product;
            voice->volume_lfo_phase = voice->volume_lfo_duration;
            table = voice->volume_lfo_table;
            if (table[0] == 0 && table[1] == 0) {
                voice->volume_lfo_table = table + table[2];
            }
            product = base_volume * (voice->volume_lfo_target >> 8);
            table = voice->volume_lfo_table;
            voice->volume_lfo_table = table + 1;
            sample = *table;
            value = (product << 9) >> 16;
            value = (value * sample) >> 15;
            if (value != voice->volume_lfo_value) {
                voice->volume_lfo_value = value;
                voice->update_flags |= AKAO_VOICE_PARAM_VOLUME;
            }
        }
    }

    if ((flags & AKAO_TRACK_FLAG_AUX_LFO) != 0) {
        if (--voice->aux_lfo_phase == 0) {
            voice->aux_lfo_phase = voice->aux_lfo_duration;
            table = voice->aux_lfo_table;
            if (table[0] == 0 && table[1] == 0) {
                voice->aux_lfo_table = table + table[2];
            }
            table = voice->aux_lfo_table;
            voice->aux_lfo_table = table + 1;
            sample = *table;
            value = ((voice->aux_lfo_target >> 8) * sample) >> 15;
            if (value != voice->aux_lfo_value) {
                voice->aux_lfo_value = value;
                voice->update_flags |= AKAO_VOICE_PARAM_VOLUME;
            }
        }
    }

    if ((flags & AKAO_TRACK_FLAG_20) != 0) {
        base_volume = (((voice[-1].pitch << 17) >> 16) * (voice->volume_base >> 8)) >> 7;
        voice->update_flags |= AKAO_VOICE_PARAM_VOLUME;
    }

    if ((voice->update_flags & AKAO_VOICE_PARAM_VOLUME) != 0) {
        int product;
        base_volume += voice->volume_lfo_value;
        product = base_volume * (D_8009CD88->timing.pitch_integer & 0x7F);
        value = ((voice->panpot >> 8) + voice->aux_lfo_value) & 0xFF;
        base_volume = product >> 7;
        switch (g_AkaoPlaybackMode) {
        case 1:
            voice->volume_left = (base_volume * g_AkaoPanVolumeTable[value]) >> 15;
            voice->volume_right = (base_volume * g_AkaoPanVolumeTable[value ^ 0xFF]) >> 15;
            break;
        case 4:
            voice->volume_left = (base_volume * g_AkaoPanVolumeTable[value]) >> 15;
            voice->volume_right = (base_volume * g_AkaoPanVolumeTable[value ^ 0xFF]) >> 15;
            if ((voice_mask & 0x00AAAAAA) != 0) {
                voice->volume_right = ~voice->volume_right;
            } else {
                voice->volume_left = ~voice->volume_left;
            }
            break;
        default:
            voice->volume_right = (unsigned int)(base_volume * g_AkaoPanVolumeTable[0x80]) >> 15;
            voice->volume_left = voice->volume_right;
            break;
        }

        if ((flags & AKAO_TRACK_FLAG_VOICE_ALLOCATED) != 0) {
            target = &g_AkaoSequencerTracks[voice->voice_index].track;
            target->volume_left = voice->volume_left;
            target->volume_right = voice->volume_right;
            base_volume = voice->pitch_slide_current >> 1;
            switch (g_AkaoPlaybackMode) {
        case 1:
            voice->volume_left = (base_volume * g_AkaoPanVolumeTable[value]) >> 15;
            voice->volume_right = (base_volume * g_AkaoPanVolumeTable[value ^ 0xFF]) >> 15;
            break;
        case 4:
            voice->volume_left = (base_volume * g_AkaoPanVolumeTable[value]) >> 15;
            voice->volume_right = (base_volume * g_AkaoPanVolumeTable[value ^ 0xFF]) >> 15;
            if ((voice_mask & 0x00AAAAAA) != 0) {
                voice->volume_right = ~voice->volume_right;
            } else {
                voice->volume_left = ~voice->volume_left;
            }
            break;
        default:
            voice->volume_right = (unsigned int)(base_volume * g_AkaoPanVolumeTable[0x80]) >> 15;
            voice->volume_left = voice->volume_right;
            break;
        }
            target->volume_left -= voice->volume_left;
            target->volume_right -= voice->volume_right;
        }
    }

    if ((flags & AKAO_TRACK_FLAG_10) != 0) {
        int pitch;
        value = g_AkaoGlobalPitchScale;
        pitch = voice[-1].pitch + voice->pitch_lfo_value + (s16)(voice->voice_mask_a >> 16);
        if (value != 0) {
            if (value < 0x80) {
                pitch += (pitch * value) >> 7;
            } else {
                pitch = (pitch * value) >> 8;
            }
        }
        voice->pitch = pitch & 0x3FFF;
        voice->update_flags |= AKAO_VOICE_PARAM_PITCH;
    } else if ((voice->update_flags & AKAO_VOICE_PARAM_PITCH) != 0) {
        int pitch;
        value = g_AkaoGlobalPitchScale;
        pitch = voice->pitch_base + voice->pitch_lfo_value + (s16)(voice->voice_mask_a >> 16);
        if (value != 0) {
            if (value < 0x80) {
                pitch += (pitch * value) >> 7;
            } else {
                pitch = (pitch * value) >> 8;
            }
        }
        voice->pitch = pitch & 0x3FFF;
    }
}

void Akao_SetVoiceKeyOn(AkaoTrack *voice, u32 voice_mask) {
    u32 flags;
    int base_volume;
    /* Two scratch values cover every stage, as in the retail register use:
     * value holds LFO results, the pan index and the pitch depth, operand
     * holds the wave sample and the pitch sum. */
    int value;
    int operand;
    s16 *table;

    base_volume = ((s16)(voice->expression_value >> 16) * (voice->volume_base >> 8)) >> 7;
    flags = voice->flags;

    if ((flags & AKAO_TRACK_FLAG_PITCH_LFO) != 0) {
        if (--voice->pitch_lfo_phase == 0) {
            voice->pitch_lfo_phase = voice->pitch_lfo_duration;
            table = voice->pitch_lfo_table;
            if (table[0] == 0 && table[1] == 0) {
                voice->pitch_lfo_table = table + table[2];
            }
            table = voice->pitch_lfo_table;
            voice->pitch_lfo_table = table + 1;
            operand = *table;
            value = (voice->pitch_lfo_depth * operand) >> 16;
            if (value != voice->pitch_lfo_value) {
                voice->pitch_lfo_value = value;
                voice->update_flags |= AKAO_VOICE_PARAM_PITCH;
                if (value >= 0) {
                    voice->pitch_lfo_value = value << 1;
                }
            }
        }
    }

    if ((flags & AKAO_TRACK_FLAG_VOLUME_LFO) != 0) {
        if (--voice->volume_lfo_phase == 0) {
            int product;
            voice->volume_lfo_phase = voice->volume_lfo_duration;
            table = voice->volume_lfo_table;
            if (table[0] == 0 && table[1] == 0) {
                voice->volume_lfo_table = table + table[2];
            }
            product = base_volume * (voice->volume_lfo_target >> 8);
            table = voice->volume_lfo_table;
            voice->volume_lfo_table = table + 1;
            operand = *table;
            value = (product << 9) >> 16;
            value = (value * operand) >> 15;
            if (value != voice->volume_lfo_value) {
                voice->volume_lfo_value = value;
                voice->update_flags |= AKAO_VOICE_PARAM_VOLUME;
            }
        }
    }

    if ((flags & AKAO_TRACK_FLAG_AUX_LFO) != 0) {
        if (--voice->aux_lfo_phase == 0) {
            voice->aux_lfo_phase = voice->aux_lfo_duration;
            table = voice->aux_lfo_table;
            if (table[0] == 0 && table[1] == 0) {
                voice->aux_lfo_table = table + table[2];
            }
            table = voice->aux_lfo_table;
            voice->aux_lfo_table = table + 1;
            operand = *table;
            value = ((voice->aux_lfo_target >> 8) * operand) >> 15;
            if (value != voice->aux_lfo_value) {
                voice->aux_lfo_value = value;
                voice->update_flags |= AKAO_VOICE_PARAM_VOLUME;
            }
        }
    }

    if ((flags & AKAO_TRACK_FLAG_20) != 0) {
        base_volume = (((voice[-1].pitch << 17) >> 16) * (voice->volume_base >> 8)) >> 7;
        voice->update_flags |= AKAO_VOICE_PARAM_VOLUME;
    }

    if ((voice->update_flags & AKAO_VOICE_PARAM_VOLUME) != 0) {
        base_volume += voice->volume_lfo_value;
        if ((voice->key_on_mask & 0x02000000) != 0) {
            value = 0x80;
        } else {
            base_volume = (base_volume * (s8)(voice->pan_target >> 8)) >> 7;
            value = ((voice->panpot >> 8) + voice->aux_lfo_value) & 0xFF;
        }

        switch (g_AkaoPlaybackMode) {
        case 1:
            voice->volume_left = (base_volume * g_AkaoPanVolumeTable[value]) >> 15;
            voice->volume_right = (base_volume * g_AkaoPanVolumeTable[value ^ 0xFF]) >> 15;
            break;
        case 4:
            voice->volume_left = (base_volume * g_AkaoPanVolumeTable[value]) >> 15;
            voice->volume_right = (base_volume * g_AkaoPanVolumeTable[value ^ 0xFF]) >> 15;
            if ((voice_mask & 0x00AAAAAA) != 0) {
                voice->volume_right = ~voice->volume_right;
            } else {
                voice->volume_left = ~voice->volume_left;
            }
            break;
        default:
            voice->volume_right = (unsigned int)(base_volume * g_AkaoPanVolumeTable[0x80]) >> 15;
            voice->volume_left = voice->volume_right;
            break;
        }
    }

    if ((flags & AKAO_TRACK_FLAG_10) != 0) {
        operand = voice[-1].pitch + voice->pitch_lfo_value + (s16)(voice->voice_mask_a >> 16);
        if ((voice->key_on_mask & 0x02000000) == 0) {
            value = (u8)(voice->pitch_offset >> 8);
            if (value != 0) {
                if (value < 0x80) {
                    operand += (operand * value) >> 7;
                } else {
                    operand = (operand * value) >> 8;
                }
            }
        }
        voice->pitch = operand & 0x3FFF;
        voice->update_flags |= AKAO_VOICE_PARAM_PITCH;
    } else if ((voice->update_flags & AKAO_VOICE_PARAM_PITCH) != 0) {
        operand = voice->pitch_base + voice->pitch_lfo_value + (s16)(voice->voice_mask_a >> 16);
        if ((voice->key_on_mask & 0x02000000) == 0) {
            value = (u8)(voice->pitch_offset >> 8);
            if (value != 0) {
                if (value < 0x80) {
                    operand += (operand * value) >> 7;
                } else {
                    operand = (operand * value) >> 8;
                }
            }
        }
        voice->pitch = operand & 0x3FFF;
    }
}

void Spu_CopyVoiceToStereoSlot(AkaoTrack *track, int stereo_voice_index) {
    int left;
    int right;
    int pan;
    int new_var;
    register int inverse_pan;
    AkaoTrack *stereo;

    left = track->volume_left;
    inverse_pan = 0x7F;
    pan = (unsigned short)track->pan_target;
    pan = ((pan << 16)) >> 24;
    inverse_pan -= pan;
    track->volume_left = ((unsigned int)(left * inverse_pan)) >> 8;
    stereo = &g_AkaoVoiceStateTable[stereo_voice_index];
    new_var = inverse_pan;
    stereo->volume_left = (left * (track->pan_target)) >> 16;
    right = (left = track->volume_right);
    track->volume_right = ((unsigned int)(right * new_var)) >> 8;
    stereo->volume_right = (right * (track->pan_target)) >> 16;
    stereo->pitch = track->pitch;
    stereo->update_flags |= (*track).update_flags;
    Akao_WriteVoiceParam(track->assigned_voice_index, (AkaoVoiceParams *)(&track->assigned_voice_index), track->flags);
    Akao_WriteVoiceParam(stereo_voice_index, (AkaoVoiceParams *)(&stereo->assigned_voice_index), track->flags);
}

void Spu_RestoreVoiceFromStereoSlot(AkaoTrack *track, int stereo_voice_index) {
    AkaoTrack *stereo;
    AkaoS16 right;

    Akao_WriteVoiceParam(track->assigned_voice_index, &AKAO_TRACK_VOICE(track), track->flags);
    stereo = &g_AkaoVoiceStateTable[stereo_voice_index];
    track->volume_left = stereo->volume_left;
    right = stereo->volume_right;
    track->update_flags |= 0x1FF93;
    track->volume_right = right;
    Akao_WriteVoiceParam(stereo_voice_index, &AKAO_TRACK_VOICE(track), track->flags);
}

void Akao_StepVoiceNote(AkaoTrack *track, unsigned mask, unsigned direct, unsigned *output) {
    unsigned bit = 1;
    int index = 0;
    unsigned requested = mask & g_AkaoCurTrack->key_on_request_mask;
    do {
        if (mask & bit) {
            Akao_SetVoiceKeyOff(track, bit, index);
            if (track->update_flags) {
                if (requested & bit) {
                    if (direct & bit) {
                        *output |= 1 << index;
                        track->assigned_voice_index = index;
                        track->update_flags |= 0x1ff93;
                    } else {
                        unsigned voice = 0;
                        AkaoVoiceEnvelopeSlot *slot = g_AkaoVoiceEnvelopeTable;
                        do {
                            if (!slot->level) {
                                track->update_flags |= 0x1ff93;
                                *output |= 1 << voice;
                                track->assigned_voice_index = voice;
                                *(short *)&slot->level = 0x7fff;
                                g_AkaoVoiceUpdateFlags |= 0x100;
                                voice = 24;
                            } else {
                                voice++;
                                slot++;
                                if (voice == 24) {
                                    track->assigned_voice_index = voice;
                                    g_AkaoCurTrack->status_flags |= 1;
                                }
                            }
                        } while (voice < 24);
                    }
                }
                if (g_AkaoVoicePortamentoResetMask & bit) {
                    track->volume_right = 0;
                    track->volume_left = 0;
                }
                if ((unsigned)track->assigned_voice_index < 24)
                    Akao_WriteVoiceParam(track->assigned_voice_index, (AkaoVoiceParams *)&track->assigned_voice_index, track->flags);
            }
            mask &= ~bit;
        }
        bit <<= 1;
        track++;
        index++;
    } while (mask);
}

/* The tracked voice ID is AkaoTrack::assigned_voice_index (+0xF0); retain
 * the original word-stride loop while exposing the shared track type in the API. */
void Akao_RemoveVoice(AkaoTrack *voices, int voiceIndex) {
    int *assignedVoiceIndex = (int *)voices;
    int i = 0;
    int unusedVoice = 0x18;

    assignedVoiceIndex += 0x3C;
    do {
        if (voiceIndex == *assignedVoiceIndex) {
            *assignedVoiceIndex = unusedVoice;
        }
        i++;
        assignedVoiceIndex += 0x47;
    } while ((unsigned int)i < 0x18);
}

void Akao_UpdateVoiceEnvelopes(s32 protectedMask) {
    AkaoVoiceEnvelopeSlot *envelope;
    s32 combinedMask;
    u32 voiceIndex;
    u32 mask;
    AkaoTrack *voiceBase;

    voiceIndex = 0;
    mask = 1;
    voiceBase = &g_AkaoVoiceStateTable[0];
    envelope = &g_AkaoVoiceEnvelopeTable[0];
    combinedMask = ((g_AkaoCurTrack[0].active_voice_mask & g_AkaoCurTrack[0].yield_voice_mask) | (g_AkaoCurTrack[1].active_voice_mask & g_AkaoCurTrack[1].yield_voice_mask)) | protectedMask;
    do {
        if (combinedMask & (mask << voiceIndex)) {
            register s32 level = 0x7FFF;
            envelope->level = level;
        } else {
            SpuGetVoiceEnvelope(voiceIndex, (unsigned short *)envelope);
            if (envelope->level == 0) {
                if (envelope) {
                    Akao_RemoveVoice(voiceBase, voiceIndex);
                    Akao_RemoveVoice(voiceBase + AKAO_VOICE_COUNT, voiceIndex);
                } else {
                    Akao_RemoveVoice(voiceBase, voiceIndex);
                    Akao_RemoveVoice(voiceBase + AKAO_VOICE_COUNT, voiceIndex);
                }
            }
        }
        voiceIndex += 1;
        envelope++;
    } while (voiceIndex < 0x18U);
}
void Akao_ProcessVoiceQueue(void)
{
  unsigned *active = &g_SpuActiveVoiceMask;
  unsigned excluded;
  unsigned secondary;
  unsigned secondary_new;
  unsigned int allocated_primary;
  unsigned primary;
  unsigned primary_new;
  unsigned result = 0;

  register unsigned bit asm("$18");
  unsigned flags;
  AkaoSequencerBank *bank;
  AkaoTrack *track;
  excluded = (*active) | g_SpuStoppedVoiceMask;
  if ((g_AkaoCurTrack[0].active_voice_mask & g_AkaoCurTrack[0].key_on_request_mask) | (g_AkaoCurTrack[1].active_voice_mask & g_AkaoCurTrack[1].key_on_request_mask))
  {
    Akao_UpdateVoiceEnvelopes(excluded);
  }
  secondary = (g_AkaoCurTrack[1].active_voice_mask & g_AkaoCurTrack[1].allocated_voice_mask) & (~(g_AkaoCurTrack[1].yield_voice_mask & excluded));
  secondary_new = (secondary & g_AkaoCurTrack[1].yield_voice_mask) & (~excluded);
  if (secondary & g_AkaoCurTrack[1].pending_voice_mask)
  {
    g_AkaoCurTrack++;
    Akao_StepVoiceNote(g_AkaoVoiceStateTable2, secondary & g_AkaoCurTrack->pending_voice_mask, secondary_new, &result);
    bank = g_AkaoCurTrack;
    secondary &= ~bank->pending_voice_mask;
    g_AkaoCurTrack--;
    bank->key_on_request_mask &= ~bank->pending_voice_mask;
  }
  allocated_primary = g_AkaoCurTrack->active_voice_mask & g_AkaoCurTrack->allocated_voice_mask;
  primary_new = secondary_new | excluded;
  primary = allocated_primary & (~(g_AkaoCurTrack->yield_voice_mask & primary_new));
  excluded = (primary & g_AkaoCurTrack->yield_voice_mask) & (~primary_new);
  if (primary & g_AkaoCurTrack->pending_voice_mask)
  {
    Akao_StepVoiceNote(g_AkaoVoiceStateTable, primary & g_AkaoCurTrack->pending_voice_mask, excluded, &result);
    bank = g_AkaoCurTrack;
    bank->key_on_request_mask &= ~bank->pending_voice_mask;
    primary &= ~bank->pending_voice_mask;
  }
  asm("" : : "r"(secondary));
  if (secondary)
  {
    g_AkaoCurTrack++;
    Akao_StepVoiceNote(g_AkaoVoiceStateTable2, secondary, secondary_new & (~excluded), &result);
    g_AkaoCurTrack->key_on_request_mask = 0;
    g_AkaoCurTrack--;
  }
  if (primary)
  {
    Akao_StepVoiceNote(g_AkaoVoiceStateTable, primary, excluded, &result);
    g_AkaoCurTrack->key_on_request_mask = 0;
  }
  primary = (*active) & g_AkaoSfxAllocatedVoiceMask;
  if (primary)
  {
    bit = 0x1000;
    track = g_AkaoVoiceChannelTable;
    result |= g_AkaoVoiceMaskScratch;
    while (primary)
    {
      if (primary & bit)
      {
        Akao_SetVoiceKeyOn(track, bit);
        if (track->update_flags)
        {
          Akao_WriteVoiceParam(track->assigned_voice_index, &track->assigned_voice_index, track->flags);
        }
        secondary_new = bit;
        primary &= ~secondary_new;
      }
      bit <<= 1;
      track++;
    }

    g_AkaoVoiceMaskScratch = 0;
  }
  primary = g_AkaoVoiceUpdateFlags;
  if (primary & 0x80)
  {
    short volume = ((short *) &g_AkaoCurTrack->volume)[1];
    SpuSetReverbModeDepth(volume, volume);
    g_AkaoVoiceUpdateFlags &= ~0x80;
  }
  if (primary & 0x10)
  {
    unsigned short clock;
    if (g_SpuActiveVoiceMask)
    {
      clock = g_AkaoTrack5ATransposeValue;
    }
    else
    {
      clock = g_AkaoCurTrack->noise_clock;
    }
    SpuSetNoiseClock(clock);
    g_AkaoVoiceUpdateFlags &= ~0x10;
  }
  if (0x100 & primary)
  {
    Akao_SetVoiceAdsr();
    Akao_SetVoiceVolume();
    Akao_SetVoiceStartAddr();
    Spu_WriteReverbEnable(g_AkaoReverbEnableMask);
    Spu_WriteNoiseEnable(g_AkaoNoiseEnableMask);
    Spu_WriteFmEnable(g_AkaoFmEnableMask);
    g_AkaoVoiceUpdateFlags &= ~0x100;
  }
  if (result)
  {
    Spu_WriteKeyOn(result);
  }
}

void Spu_VoiceMaskCompose(AkaoTrack *track, int *mask_out, int mask, int mask_keep) {
    int bit;
    int idx;

    bit = 1;
    do {
        if ((mask & bit) != 0) {
            idx = track->assigned_voice_index;
            if ((unsigned int) idx < 0x18) {
                *mask_out |= 1 << idx;
            }
        }
        mask &= ~bit;
        track++;
        bit <<= 1;
    } while (mask != 0);

    *mask_out &= mask_keep;
}

void Akao_SetVoicePitch(void) {
    unsigned secondary, primary, keep;
    AkaoSequencerBank *bank;
    int result = 0;
    keep = ~(g_SpuActiveVoiceMask | g_SpuStoppedVoiceMask);
    secondary = g_AkaoCurTrack[1].active_voice_mask & g_AkaoCurTrack[1].key_off_request_mask;
    if (secondary & g_AkaoCurTrack[1].pending_voice_mask) {
        g_AkaoCurTrack++;
        Spu_VoiceMaskCompose(g_AkaoVoiceStateTable2, &result, secondary & g_AkaoCurTrack->pending_voice_mask, keep);
        bank = g_AkaoCurTrack;
        secondary &= ~bank->pending_voice_mask;
        g_AkaoCurTrack--;
        bank->key_off_request_mask &= ~bank->pending_voice_mask;
    }
    primary = g_AkaoCurTrack->active_voice_mask & g_AkaoCurTrack->key_off_request_mask;
    if (primary & g_AkaoCurTrack->pending_voice_mask) {
        Spu_VoiceMaskCompose(g_AkaoVoiceStateTable, &result, primary & g_AkaoCurTrack->pending_voice_mask, keep);
        bank = g_AkaoCurTrack;
        bank->key_off_request_mask &= ~bank->pending_voice_mask;
        primary &= ~bank->pending_voice_mask;
    }
    if (secondary) {
        g_AkaoCurTrack++;
        Spu_VoiceMaskCompose(g_AkaoVoiceStateTable2, &result, secondary, keep);
        g_AkaoCurTrack->key_off_request_mask = 0;
        g_AkaoCurTrack--;
    }
    if (primary) {
        Spu_VoiceMaskCompose(g_AkaoVoiceStateTable, &result, primary, keep);
        g_AkaoCurTrack->key_off_request_mask = 0;
    }
    result |= g_SpuPendingKeyOffMask;
    g_SpuPendingKeyOffMask = 0;
    if (result) Spu_WriteKeyOff(result);
}

void Seq_MarkTrack34MaskDirty(void) {
    g_AkaoVoiceUpdateFlags |= 0x100;
}

void Akao_SetVoiceVolume(void) {
    unsigned secondary, primary, keep;
    int result = 0;
    keep = ~(g_SpuActiveVoiceMask | g_SpuStoppedVoiceMask);
    secondary = g_AkaoCurTrack[1].active_voice_mask & g_AkaoCurTrack[1].key_off_dirty_mask;
    if (secondary & g_AkaoCurTrack[1].pending_voice_mask) {
        g_AkaoCurTrack++;
        Spu_VoiceMaskCompose(g_AkaoVoiceStateTable2, &result, secondary & g_AkaoCurTrack->pending_voice_mask, keep);
        secondary &= ~g_AkaoCurTrack->pending_voice_mask;
        g_AkaoCurTrack--;
    }
    primary = g_AkaoCurTrack->active_voice_mask & g_AkaoCurTrack->key_off_dirty_mask;
    if (primary & g_AkaoCurTrack->pending_voice_mask) {
        Spu_VoiceMaskCompose(g_AkaoVoiceStateTable, &result, primary & g_AkaoCurTrack->pending_voice_mask, keep);
        primary &= ~g_AkaoCurTrack->pending_voice_mask;
    }
    if (secondary) {
        g_AkaoCurTrack++;
        Spu_VoiceMaskCompose(g_AkaoVoiceStateTable2, &result, secondary, keep);
        g_AkaoCurTrack--;
    }
    if (primary) Spu_VoiceMaskCompose(g_AkaoVoiceStateTable, &result, primary, keep);
    result |= g_AkaoTrack34Mask;
    g_AkaoNoiseEnableMask = result;
    g_AkaoVoiceUpdateFlags |= 0x100;
}

void Seq_MarkTrack38MaskDirty(void) {
    g_AkaoVoiceUpdateFlags |= 0x100;
}

void Akao_SetVoiceAdsr(void) {
    unsigned secondary, primary, keep;
    int result = 0;
    keep = ~(g_SpuActiveVoiceMask | g_SpuStoppedVoiceMask);
    secondary = g_AkaoCurTrack[1].active_voice_mask & g_AkaoCurTrack[1].volume_dirty_mask;
    if (secondary & g_AkaoCurTrack[1].pending_voice_mask) {
        g_AkaoCurTrack++;
        Spu_VoiceMaskCompose(g_AkaoVoiceStateTable2, &result, secondary & g_AkaoCurTrack->pending_voice_mask, keep);
        secondary &= ~g_AkaoCurTrack->pending_voice_mask;
        g_AkaoCurTrack--;
    }
    primary = g_AkaoCurTrack->active_voice_mask & g_AkaoCurTrack->volume_dirty_mask;
    if (primary & g_AkaoCurTrack->pending_voice_mask) {
        Spu_VoiceMaskCompose(g_AkaoVoiceStateTable, &result, primary & g_AkaoCurTrack->pending_voice_mask, keep);
        primary &= ~g_AkaoCurTrack->pending_voice_mask;
    }
    if (secondary) {
        g_AkaoCurTrack++;
        Spu_VoiceMaskCompose(g_AkaoVoiceStateTable2, &result, secondary, keep);
        g_AkaoCurTrack--;
    }
    if (primary) Spu_VoiceMaskCompose(g_AkaoVoiceStateTable, &result, primary, keep);
    result |= g_AkaoTrack38Mask;
    g_AkaoReverbEnableMask = result;
    g_AkaoVoiceUpdateFlags |= 0x100;
}

void Seq_MarkTrack3CMaskDirty(void) {
    g_AkaoVoiceUpdateFlags |= 0x100;
}

void Akao_SetVoiceStartAddr(void) {
    unsigned secondary, primary, keep;
    int result = 0;
    keep = ~(g_SpuActiveVoiceMask | g_SpuStoppedVoiceMask);
    secondary = g_AkaoCurTrack[1].active_voice_mask & g_AkaoCurTrack[1].adsr_dirty_mask;
    if (secondary & g_AkaoCurTrack[1].pending_voice_mask) {
        g_AkaoCurTrack++;
        Spu_VoiceMaskCompose(g_AkaoVoiceStateTable2, &result, secondary & g_AkaoCurTrack->pending_voice_mask, keep);
        secondary &= ~g_AkaoCurTrack->pending_voice_mask;
        g_AkaoCurTrack--;
    }
    primary = g_AkaoCurTrack->active_voice_mask & g_AkaoCurTrack->adsr_dirty_mask;
    if (primary & g_AkaoCurTrack->pending_voice_mask) {
        Spu_VoiceMaskCompose(g_AkaoVoiceStateTable, &result, primary & g_AkaoCurTrack->pending_voice_mask, keep);
        primary &= ~g_AkaoCurTrack->pending_voice_mask;
    }
    if (secondary) {
        g_AkaoCurTrack++;
        Spu_VoiceMaskCompose(g_AkaoVoiceStateTable2, &result, secondary, keep);
        g_AkaoCurTrack--;
    }
    if (primary) Spu_VoiceMaskCompose(g_AkaoVoiceStateTable, &result, primary, keep);
    result |= g_AkaoTrack3CMask;
    g_AkaoFmEnableMask = result;
    g_AkaoVoiceUpdateFlags |= 0x100;
}
