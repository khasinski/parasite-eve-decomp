#include "pe1/akao.h"

int Spu_UploadStreamBlockB(unsigned bank, unsigned char *cursor)
{
    unsigned char *instruments;
    unsigned size, start, count, address;
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
    Spu_WriteRegChecked(address);
    Spu_UploadWithPrepare((int)cursor,size);
    Spu_RebaseStreamAddrs(instruments,address,count);
    /* Cache the relocated records after the SPU payload upload. */
    address = bank << 10;
    dest = (unsigned *)((unsigned char *)D_800B4900 + address);
    size = count << 4;
    do {
        --size;
        *dest++ = *(unsigned *)instruments;
        instruments += 4;
    } while (size);
    return 0;
}
