/* CC1_PROFILE: build0001 */
extern int g_AkaoSeqLoopCounter;

void Seq_SelectPlaybackBank(int *arg0);

void Seq_SelectPlaybackBankWithCountdown(int *arg0) {
    int raw;
    register int value asm("$2");

    Seq_SelectPlaybackBank(arg0);
    raw = arg0[4];
    value = 0;
    if (raw != 0) {
        value = raw - 1;
    }
    g_AkaoSeqLoopCounter = value;
}
