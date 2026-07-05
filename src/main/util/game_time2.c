extern volatile int D_800A76A0;
extern volatile int g_GameTimeTable;
extern volatile int D_800A76A8;

extern volatile int D_800A76B8;
extern volatile int g_PlayTimeSeconds;
extern volatile int D_800A76C0;

extern volatile int D_800A76AC;
extern volatile int D_800A76B0;
extern volatile int D_800A76B4;

typedef unsigned int u32;

void GameTime_InitCounter0(void) {
    int value;

    value = 0x1499700;
    D_800A76A8 = 0;
    D_800A76A8 = value;
    value = 1;
    g_GameTimeTable = 0;
    D_800A76A0 = 0;
    D_800A76A0 = value;
}

void GameTime_InitCounter2(void) {
    int value;

    value = 1;
    g_PlayTimeSeconds = 0;
    D_800A76C0 = 0;
    D_800A76B8 = value;
}

void GameTime_InitCounter1(void) {
    int value;

    value = 1;
    D_800A76B0 = 0;
    D_800A76B4 = 0;
    D_800A76AC = value;
}

void Util_PackTime(u32 *src, u32 *out) {
    register u32 value asm("$7");
    u32 magic;
    u32 mask;
    u32 word;
    register u32 tmp asm("$3");
    u32 last;
    u32 packed;

    value = src[1];
    magic = 0x88888889;
    value %= 60;
    mask = 0xFFFFF;
    word = *out;
    word &= 0xFFF00000;
    tmp = value * 7;
    tmp <<= 5;
    tmp += value;
    packed = ((tmp * 15) << 6) & mask;
    word |= packed;
    *out = word;

    value = src[1];
    value %= 60;
    word &= 0xFC0FFFFF;
    packed = ((value * 3600) & 0x3F) << 20;
    word |= packed;
    *out = word;

    last = src[1];
    last %= 60;
    word &= 0x03FFFFFF;
    packed = (last * 15) << 28;
    word |= packed;
    *out = word;
}
