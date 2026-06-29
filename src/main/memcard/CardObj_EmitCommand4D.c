void CardObj_EmitCommand4D(void *arg0)
{
    int value = *(int *)((char *)arg0 + 0x20);

    *((unsigned char *)arg0 + 0x36) = 0x4D;
    *((unsigned char *)arg0 + 0x35) = 6;
    *(int *)((char *)arg0 + 0x2C) = value;
}
