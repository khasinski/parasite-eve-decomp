extern unsigned char g_DrawEnabled;
extern short g_SeqElapsed;

int Seq_GetElapsed(void) {
    if (g_DrawEnabled != 0) {
        return g_SeqElapsed;
    }
    return -1;
}
