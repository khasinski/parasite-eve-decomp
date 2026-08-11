extern u8 D_801932C0;
extern u8 D_801932C1;
extern u8 D_801932C2;
extern u8 D_801932C4;
extern u8 D_801932C5;
extern u8 D_801932C6;
extern s16 D_801932C8;
extern s16 D_801932CA;
extern u8 D_801932D0;
extern u8 D_801932D1;
extern u8 D_801932D2;
extern u8 D_801932D4;
extern u8 D_801932D5;
extern u8 D_801932D6;
extern s16 D_801932D8;
extern s16 D_801932DA;

void ROOMLIB_INIT_EFFECT_PALETTE_B_NAME(char *obj, void *unused, char *state) {
    char *owner;
    RoomEffectWords8 *words;

    func_800C2B40(state);
    *(void **)(state + 0x24) = func_8006DC18(0xB);

    owner = *(char **)(obj + 8);
    *(char **)state = owner;
    words = *(RoomEffectWords8 **)(owner + 0x238);
    *(RoomEffectWords8 *)(state + 4) = *words;
    *(s16 *)(state + 0x28) = 0x28;
    *(s16 *)(state + 0x2A) = 0;
    *(s16 *)(state + 0x2C) = 0;

    D_801932D4 = 4;
    D_801932D5 = 1;
    D_801932D8 = 0;
    D_801932DA = 0x80;
    D_801932D0 = 0x80;
    D_801932D1 = 0x80;
    D_801932D2 = 0x80;
    D_801932D6 = 0;
    D_801932C4 = 8;
    D_801932C5 = 2;
    D_801932C8 = 0;
    D_801932CA = 0x30;
    D_801932C0 = 0x80;
    D_801932C1 = 0x80;
    D_801932C2 = 0x80;
    D_801932C6 = 0;
}
