void CardObj_SwapByteField(unsigned char *ptr) {
    unsigned char value = ptr[0x36];
    ptr[0x36] = 0;
    ptr[0x37] = value;
}
