typedef unsigned char u8;
typedef unsigned int u32;

u32 Save_CalcCrc16(u32 size, u8 *data) {
    u32 crc = 0xFFFF;
    u32 i = 0;

    size &= 0xFFFF;
    if (size != 0) {
        do {
            u32 bit = 0;

            crc ^= data[i & 0xFFFF] << 8;
            do {
                if (crc & 0x8000) {
                    crc = (crc << 1) ^ 0x1021;
                } else {
                    crc <<= 1;
                }
                bit++;
            } while ((bit & 0xFFFF) < 8);
            i++;
        } while ((i & 0xFFFF) < size);
    }

    return ~crc & 0xFFFF;
}
