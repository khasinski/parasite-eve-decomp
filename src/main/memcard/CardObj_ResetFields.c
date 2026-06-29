void CardObj_ResetFields(char *arg0) {
    int count;
    int value;
    char *ptr;

    if (*(unsigned char *)(arg0 + 0x49) != 0) {
        ptr = arg0 + 0x5D;
        value = 0xFF;
        count = 5;
        *(char *)(arg0 + 0x49) = 0;
        *(char *)(arg0 + 0x46) = 0;
        *(short *)(arg0 + 0xE6) = 0;
        *(int *)(arg0 + 0x14) = 0;
        *(int *)(arg0 + 0x18) = 0;
        *(char *)(arg0 + 0xE3) = 0;
        *(char *)(arg0 + 0xE4) = 0;
        *(short *)(arg0 + 0xE6) = 0;
        *(char *)(arg0 + 0xE9) = 0;
        *(char *)(arg0 + 0xEA) = 0;
        *(int *)(arg0 + 0x0) = 0;
        *(int *)(arg0 + 0x4) = 0;
        *(int *)(arg0 + 0x8) = 0;

        do {
            *ptr = value;
            count -= 1;
            ptr += 1;
        } while (count >= 0);
    }
}
