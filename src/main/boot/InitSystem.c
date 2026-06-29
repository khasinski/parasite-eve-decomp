void ResetCallback(void);
void Render_ResetScene(int arg0, int arg1);
void SpuInit(void);
void InitGeom(void);
void SetGeomOffset(int x, int y);
void SetGeomScreen(int h);
void MemCard_InitManager(void);
void Boot_InitMemCard(void);
void CdRom_InitDsReadSystem(void);
void CdRom_SetReadCallback(int callback);

void InitSystem(void) {
    ResetCallback();
    Render_ResetScene(0x140, 0xE0);
    SpuInit();
    InitGeom();
    SetGeomOffset(0xA0, 0x70);
    SetGeomScreen(0xF0);
    MemCard_InitManager();
    Boot_InitMemCard();
    CdRom_InitDsReadSystem();
    CdRom_SetReadCallback(0);
}
