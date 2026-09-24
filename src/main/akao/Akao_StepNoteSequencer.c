#include "pe1/akao.h"
#include "pe1/akao/voice_state.h"

int Akao_StepNoteSequencer(unsigned char *cursor, unsigned size)
{
    register unsigned char *input asm("$16") = cursor;
    register unsigned available asm("$18") = size;
    unsigned char *instruments;
    unsigned payload_size;
    unsigned count;
    unsigned address;
    unsigned address_offset;
    unsigned upload_size;
    register unsigned after_header asm("$2");
    register unsigned char *rebase_instruments asm("$4");
    register unsigned rebase_address asm("$5");
    register unsigned next_address asm("$2");
    register unsigned remaining_payload asm("$2");
    unsigned start;
    unsigned *dest;
    register unsigned *dest_base asm("$2");
    unsigned result;
    register unsigned mode asm("$3");
    register unsigned clear_bits asm("$4");

    if (D_8009D270 & 1) {
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

        address = 0x8000;
        if (count < 0x31) {
            if (g_AkaoCurTrack[1].active_voice_mask != 0) {
                if ((g_AkaoCurTrack[1].status_flags & 0x100) == 0) {
                    address = 0x38000;
                    g_AkaoVoiceKeyOnState |= 0x100;
                    goto address_selected;
                }
            } else {
                asm volatile("" : "=r"(address) : "0"(address));
                address = 0x8000;
                if (g_AkaoCurTrack[0].active_voice_mask != 0) {
                    if ((g_AkaoCurTrack[0].status_flags & 0x100) == 0) {
                        address = 0x38000;
                        g_AkaoVoiceKeyOnState |= 0x100;
                        goto address_selected;
                    }
                }
            }
        }
        g_AkaoVoiceKeyOnState &= ~0x100U;
address_selected:
        Spu_WriteRegChecked(address);
        Spu_UploadWithPrepare((int)input, upload_size);
        rebase_instruments = instruments;
        rebase_address = address;
        next_address = address + upload_size;
        D_8009D2BC = next_address;
        remaining_payload = payload_size - upload_size;
        D_8009D2E4 = remaining_payload;
        Spu_RebaseStreamAddrs(rebase_instruments, rebase_address, count);
        address_offset = address == 0x8000 ? 0x20 : 0x50;
        address_offset <<= 6;
        dest_base = D_800B2900;
        dest = (unsigned *)(address_offset + (unsigned)dest_base);
        upload_size = count << 4;
        do {
            --upload_size;
            *dest++ = *(unsigned *)instruments;
            instruments += 4;
        } while (upload_size);
        clear_bits = ~1U;
        mode = D_8009D270;
        result = D_8009D2E4;
        mode &= clear_bits;
        D_8009D270 = mode;
    } else {
        Spu_WriteRegChecked(D_8009D2BC);
        upload_size = available;
        if (upload_size >= D_8009D2E4) upload_size = D_8009D2E4;
        Spu_UploadWithPrepare((int)input, upload_size);
        D_8009D2BC += upload_size;
        D_8009D2E4 -= upload_size;
        result = D_8009D2E4;
    }
    return result;
}
