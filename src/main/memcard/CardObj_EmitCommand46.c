void CardObj_EmitCommand46(unsigned char *arg0, unsigned char arg1) {
    arg0[0x36] = 0x46;
    *(unsigned char **)(arg0 + 0x2C) = arg0 + 0x24;
    arg0[0x24] = arg1;
    arg0[0x35] = 1;
}
