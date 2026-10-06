/* PSY-Q LIBSND SSINIT_C: SsInit. */
void GameTime_InitCounter0(void);
void GameTime_InitCounter2(void);
void GameTime_InitCounter1(void);

void SsInit(void) {
    GameTime_InitCounter0();
    GameTime_InitCounter2();
    GameTime_InitCounter1();
}
