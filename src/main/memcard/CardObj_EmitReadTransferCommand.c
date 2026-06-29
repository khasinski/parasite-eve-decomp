void CardObj_EmitCommand4C(void *arg0, unsigned char arg1);
void CardObj_EmitCommand46(void *arg0, unsigned char arg1);
void CardObj_EmitCommand47(void *arg0, unsigned char arg1);
void CardObj_EmitCommand4B(void *arg0);

void CardObj_EmitReadTransferCommand(void *arg0)
{
    int state = *((unsigned char *)arg0 + 0x46);

    switch (state) {
    case 2:
        CardObj_EmitCommand4C(arg0, *((unsigned char *)arg0 + 0x47));
        break;
    case 3:
        CardObj_EmitCommand46(arg0, *((unsigned char *)arg0 + 0x47));
        break;
    case 4:
        if (*((unsigned char *)arg0 + 0x48) == 0) {
            CardObj_EmitCommand47(arg0, *((unsigned char *)arg0 + 0x47));
        } else {
            CardObj_EmitCommand4B(arg0);
        }
        break;
    }
}
