int Render_DrawSprite(void);
int Scene_CheckBattleFlag(void);
int Scene_IsBattleMode(void);
int Geo_BuildMeshList(void);
int Render_StepFade(void);
int Render_ApplyScreenTint(void);

int Render_Update(void) {
    Render_DrawSprite();
    Scene_CheckBattleFlag();
    Scene_IsBattleMode();
    Geo_BuildMeshList();
    Render_StepFade();
    Render_ApplyScreenTint();
    return 0;
}
