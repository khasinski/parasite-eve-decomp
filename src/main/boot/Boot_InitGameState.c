void Boot_InitMemoryLayout(void);
void Akao_ClearVoiceBank(void);

void Boot_InitGameState(void) {
    Boot_InitMemoryLayout();
    Akao_ClearVoiceBank();
}
