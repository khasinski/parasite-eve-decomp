typedef signed short s16;
typedef unsigned char u8;
typedef unsigned int u32;

typedef struct {
    s16 x;
    s16 y;
    s16 w;
    s16 h;
} RECT;

extern u32 g_GameState;
extern unsigned char g_DiscChangeFlags;

void ClearImage(RECT *rect, u8 r, u8 g, u8 b);
void DrawSync(int arg0);
void Render_PrepareFrame(void);

void Gpu_ClearOnFlag(void) {
    u32 *state = &g_GameState;

    if (*state & 0x08000000) {
        RECT rect;
        unsigned int value;
        unsigned char byte;

        rect.x = 0;
        rect.y = 0;
        rect.w = 0x140;
        rect.h = 0x1C0;
        ClearImage(&rect, 0, 0, 1);
        DrawSync(0);
        Render_PrepareFrame();
        byte = g_DiscChangeFlags;
        value = *state;
        g_DiscChangeFlags = byte | 2;
        *state = value & 0xF7FFFDFF;
    }
}
