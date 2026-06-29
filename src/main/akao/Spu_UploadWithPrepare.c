/* CC1_PROFILE: build0001 */
void Spu_PrepareTransfer(void);
void Spu_UploadToSpu(int arg0, int arg1);

void Spu_UploadWithPrepare(int arg0, int arg1) {
    Spu_PrepareTransfer();
    Spu_UploadToSpu(arg0, arg1);
}
