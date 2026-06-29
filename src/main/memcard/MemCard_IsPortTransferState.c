typedef unsigned char u8;

extern u8 D_800A0ED5[];

int MemCard_IsPortTransferState(int arg0) {
    u8 value = D_800A0ED5[arg0 * 0x418];

    return (value == 3) || (value == 8) || (value == 10);
}
