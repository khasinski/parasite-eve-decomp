void CardObj_SetPayload4D(void *arg0, int arg1, unsigned char arg2)
{
    *(int *)((char *)arg0 + 0x28) = arg1;
    *((unsigned char *)arg0 + 0x34) = arg2;
}
