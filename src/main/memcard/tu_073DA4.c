void CardObj_EmitCommand45(void *arg0);
void CardObj_EmitCommand4C(void *arg0, unsigned char arg1);
void CardObj_EmitCommand47(void *arg0, unsigned char arg1);

void CardObj_SetPayload4D(void *arg0, int arg1, unsigned char arg2)
{
    *(int *)((char *)arg0 + 0x28) = arg1;
    *((unsigned char *)arg0 + 0x34) = arg2;
}

void CardObj_SetPayloadCommand(void *arg0, unsigned char arg1, int arg2, unsigned char arg3)
{
    *((unsigned char *)arg0 + 0x36) = arg1;
    *(int *)((char *)arg0 + 0x2C) = arg2;
    *((unsigned char *)arg0 + 0x35) = arg3;
}

void CardObj_EmitReadCommandForState(void *arg0)
{
    int state = *((unsigned char *)arg0 + 0x46);

    switch (state) {
    case 2:
        CardObj_EmitCommand45(arg0);
        break;
    case 3:
        CardObj_EmitCommand4C(arg0, *((unsigned char *)arg0 + 0xE4));
        break;
    case 4:
        CardObj_EmitCommand47(arg0, *((unsigned char *)arg0 + 0x47));
        break;
    }
}
