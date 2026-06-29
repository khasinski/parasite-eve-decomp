/* CC1_PROFILE: build0001 */
/* MASPSX_FLAGS: --stack-return-delay */

int Sys_VSyncTimeout(int arg0) __asm__("func_800811E4");

extern int g_GameState;

asm(".globl func_8006E7E8");
asm("func_8006E7E8 = CdRom_PollReady");

int CdRom_PollReady(void) {
    int scratch;
    int status;
    int *state;

    status = Sys_VSyncTimeout((int)&scratch);
    if ((unsigned int)(status + 1) < 2U) {
        state = &g_GameState;
        *state &= 0xFEFFBFFF;
    }
    return status;
}
