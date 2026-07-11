typedef unsigned char u8;

typedef struct CardObj {
    u8 pad_00[0x36];
    u8 command;
    u8 saved_command;
} CardObj;

void CardObj_SwapByteField(CardObj *obj) {
    u8 command;

    command = obj->command;
    obj->command = 0;
    obj->saved_command = command;
}
