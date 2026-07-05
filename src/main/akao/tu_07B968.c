typedef unsigned short u16;

void Seq_StartNestedStreams(void *arg0, void *arg1, void *arg2);

void Spu_ManageVoices(int arg0, int arg1);

void Seq_StartRelativeNestedStream(void *arg0) {
    char *base;
    u16 offset;
    void *arg1;
    void *arg2;

    base = *(char **)((char *)arg0 + 4);
    offset = *(u16 *)base;
    if (offset != 0xFFFF) {
        arg1 = (char *)(offset + (int)base) + 4;
    } else {
        arg1 = 0;
    }

    offset = *(u16 *)(base + 2);
    arg2 = 0;
    if (offset != 0xFFFF) {
        arg2 = (char *)(offset + (int)*(char **)((char *)arg0 + 4)) + 4;
    }

    *(void **)((char *)arg0 + 4) = *(void **)((char *)arg0 + 0x14);
    Seq_StartNestedStreams(arg0, arg1, arg2);
}

void Spu_ManageVoicesCmd(int *arg0) {
    Spu_ManageVoices(arg0[1], arg0[2]);
}
