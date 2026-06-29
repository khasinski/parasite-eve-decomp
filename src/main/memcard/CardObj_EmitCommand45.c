void CardObj_EmitCommand45(unsigned char *arg0) {
    arg0[0x36] = 0x45;
    *(int *)(arg0 + 0x2C) = 0;
    arg0[0x35] = 0;
}
