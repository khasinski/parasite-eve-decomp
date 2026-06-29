void CardObj_EmitReadIdCommand(void *arg0)
{
    int state = *((unsigned char *)arg0 + 0x46);

    switch (state) {
    case 2:
        *((unsigned char *)arg0 + 0x36) = 0x44;
        *(void **)((char *)arg0 + 0x2C) = (char *)arg0 + 0x51;
        *((unsigned char *)arg0 + 0x35) = state;
        break;
    case 3:
        *((unsigned char *)arg0 + 0x36) = 0x4D;
        *(void **)((char *)arg0 + 0x2C) = (char *)arg0 + 0x5D;
        *((unsigned char *)arg0 + 0x35) = 6;
        break;
    }
}
