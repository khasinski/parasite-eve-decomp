void SeqOp_StopVoice(void *ptr);

void SeqOp_Noop(void *ptr) {
    SeqOp_StopVoice(ptr);
}
