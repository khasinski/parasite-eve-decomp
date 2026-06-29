void SeqOp_ClearTrack34Mask(void *ptr);
void SeqOp_ClearTrack3CMask(void *ptr, int arg);
void SeqOp_ClearMask(void *ptr, int arg);

void SeqOp_StopAndClearTrack(void *ptr, int arg) {
    *(int *)((char *)ptr + 0x38) &= -0x38;
    SeqOp_ClearTrack34Mask(ptr);
    SeqOp_ClearTrack3CMask(ptr, arg);
    SeqOp_ClearMask(ptr, arg);
    *(unsigned short *)((char *)ptr + 0x84) &= 0xFFFA;
}
