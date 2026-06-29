/* CC1_PROFILE: build0001 */
extern unsigned char g_DrawEnabled;
extern short g_SeqElapsed;

asm(".globl func_8006EBE4");
asm("func_8006EBE4 = Seq_GetElapsed");

int Seq_GetElapsed(void) {
    if (g_DrawEnabled != 0) {
        return g_SeqElapsed;
    }
    return -1;
}
