int Spu_UploadSampleBlock(int arg0, int arg1);

void Spu_UploadSampleBlockBlocking(int arg0, int arg1) {
    while (Spu_UploadSampleBlock(arg0, arg1) == 1) {
    }
}
