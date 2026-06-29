void Seq_StartNestedStreams(void *arg0, int arg1, int arg2);

void Seq_StartDefaultNestedStream(int *arg0) {
    int old1 = arg0[1];
    int old2 = arg0[2];

    arg0[1] = 0x400;
    arg0[2] = 0x1000000;
    arg0[3] = 0x80;
    arg0[4] = 0x7F;
    Seq_StartNestedStreams(arg0, old1, old2);
}
