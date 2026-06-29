void CardObj_SetPayloadCommand(void *arg0, unsigned char arg1, int arg2, unsigned char arg3)
{
    *((unsigned char *)arg0 + 0x36) = arg1;
    *(int *)((char *)arg0 + 0x2C) = arg2;
    *((unsigned char *)arg0 + 0x35) = arg3;
}
