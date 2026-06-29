typedef unsigned char u8;

extern u8 D_800A0EDE[];

int MemCard_GetPortFileCount(int arg0) {
    return D_800A0EDE[arg0 * 0x418];
}
