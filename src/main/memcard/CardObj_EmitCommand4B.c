void CardObj_EmitCommand4B(unsigned char *arg0) {
    arg0[0x36] = 0x4B;
    *(int *)(arg0 + 0x2C) = 0;
    arg0[0x35] = 0;
}
