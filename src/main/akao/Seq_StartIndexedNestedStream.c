void Akao_LoadSamplePairFromIndex(int *arg0, int *arg1, int arg2);
void Seq_StartNestedStreams(void *arg0, int arg1, int arg2);

void Seq_StartIndexedNestedStream(int *arg0) {
    int old1;
    int old2;

    Akao_LoadSamplePairFromIndex(&old1, &old2, arg0[1]);
    Seq_StartNestedStreams(arg0, old1, old2);
}
