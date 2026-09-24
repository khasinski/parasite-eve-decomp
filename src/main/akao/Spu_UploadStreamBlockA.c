#include "pe1/akao.h"

int Spu_UploadStreamBlockA(unsigned bank, unsigned char *cursor, unsigned size)
{
    register unsigned char *input asm("$16") = cursor;
    register unsigned available asm("$18") = size;
    unsigned char *instruments;
    unsigned payload_size;
    unsigned count;
    unsigned address;
    unsigned upload_size;
    register unsigned after_header asm("$2");
    unsigned address_base;
    unsigned address_offset;
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

    if (D_8009D270 & 2) {
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
        address_base = 0x4f000;
        address_offset = ((bank * 5) << 13);
        address = address_offset + address_base;
        Spu_WriteRegChecked(address);
        Spu_UploadWithPrepare((int)input, upload_size);
        rebase_instruments = instruments;
        rebase_address = address;
        next_address = rebase_address + upload_size;
        D_8009D1EC = next_address;
        remaining_payload = payload_size - upload_size;
        D_8009D204 = remaining_payload;
        Spu_RebaseStreamAddrs(rebase_instruments, rebase_address, count);
        address = bank << 10;
        dest_base = D_800B4D00;
        dest = (unsigned *)(address + (unsigned)dest_base);
        upload_size = count << 4;
        do {
            --upload_size;
            *dest++ = *(unsigned *)instruments;
            instruments += 4;
        } while (upload_size);
        clear_bits = ~2U;
        mode = D_8009D270;
        result = D_8009D204;
        mode &= clear_bits;
        D_8009D270 = mode;
    } else {
        Spu_WriteRegChecked(D_8009D1EC);
        upload_size = available;
        if (upload_size >= D_8009D204) upload_size = D_8009D204;
        Spu_UploadWithPrepare((int)input, upload_size);
        D_8009D1EC += upload_size;
        D_8009D204 -= upload_size;
        result = D_8009D204;
    }
    return result;
}
