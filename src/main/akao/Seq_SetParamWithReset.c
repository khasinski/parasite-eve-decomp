/* CC1_PROFILE: build0001 */
void Seq_GetGlobalPitch(unsigned int *out);
void SPU_ReadRegister(int arg0);
int SPU_StepDmaRead(int arg0);

void Seq_SetParamWithReset(unsigned int arg0) {
    unsigned int value;

    Seq_GetGlobalPitch(&value);
    if (value != arg0) {
        SPU_ReadRegister(0);
        SPU_StepDmaRead(arg0 | 0x100);
        SPU_ReadRegister(1);
    }
}
