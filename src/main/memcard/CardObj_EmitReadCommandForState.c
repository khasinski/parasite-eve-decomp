void CardObj_EmitCommand45(void *arg0);
void CardObj_EmitCommand4C(void *arg0, unsigned char arg1);
void CardObj_EmitCommand47(void *arg0, unsigned char arg1);

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
