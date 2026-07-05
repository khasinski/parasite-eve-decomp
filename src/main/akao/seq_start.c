void Akao_LoadSamplePairFromIndex(int *arg0, int *arg1, int arg2);
void Seq_StartNestedStreams(void *arg0, int arg1, int arg2);

void Seq_StartIndexedNestedStreamWithDefaults(int *arg0) {
    int old1;
    int old2;

    Akao_LoadSamplePairFromIndex(&old1, &old2, arg0[1]);
    arg0[2] = 0x2000000;
    arg0[3] = 0x80;
    arg0[4] = 0x7F;
    Seq_StartNestedStreams(arg0, old1, old2);
}

void Seq_StartIndexedNestedStream(int *arg0) {
    int old1;
    int old2;

    Akao_LoadSamplePairFromIndex(&old1, &old2, arg0[1]);
    Seq_StartNestedStreams(arg0, old1, old2);
}
