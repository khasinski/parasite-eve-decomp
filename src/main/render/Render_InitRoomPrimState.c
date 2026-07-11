typedef signed short s16;
typedef unsigned char u8;

extern s16 D_800BEA40[];
extern s16 D_800BEA60[];
extern u8 D_800BD025;
extern u8 D_800BD026;
extern u8 D_800BD027;

static int Render_ClampByte(int value) {
    if (value < 0) {
        return 0;
    }
    if (value >= 0x100) {
        return 0xFF;
    }
    return value;
}

static s16 Render_ScaleRoomColour(int value, int colour) {
    return (s16)((value * colour) >> 8);
}

static void Render_LoadRoomColorMatrix(void) {
    int *matrix;
    int w0;
    int w1;
    int w2;
    int w3;
    int w4;

    matrix = (int *)D_800BEA60;
    w0 = matrix[0];
    w1 = matrix[1];
    w2 = matrix[2];
    w3 = matrix[3];
    w4 = matrix[4];

    asm volatile("ctc2 %0,$16" : : "r"(w0));
    asm volatile("ctc2 %0,$17" : : "r"(w1));
    asm volatile("ctc2 %0,$18" : : "r"(w2));
    asm volatile("ctc2 %0,$19" : : "r"(w3));
    asm volatile("ctc2 %0,$20" : : "r"(w4));
}

int Render_InitRoomPrimState(u8 *entity) {
    int shade_a;
    int shade_b;

    D_800BEA40[0] = 0;
    D_800BEA40[1] = 0x1000;
    D_800BEA40[2] = 0;
    D_800BEA40[3] = 0;
    D_800BEA40[4] = -0x1000;
    D_800BEA40[5] = 0;
    D_800BEA40[6] = 0;
    D_800BEA40[7] = 0;
    D_800BEA40[8] = 0;

    shade_a = Render_ClampByte(entity[0x88] + entity[0x8A]) << 4;
    shade_b = Render_ClampByte(entity[0x88] + entity[0x89]) << 4;

    D_800BEA60[0] = Render_ScaleRoomColour(shade_a, D_800BD025);
    D_800BEA60[1] = Render_ScaleRoomColour(shade_b, D_800BD025);
    D_800BEA60[2] = 0;
    D_800BEA60[3] = Render_ScaleRoomColour(shade_a, D_800BD026);
    D_800BEA60[4] = Render_ScaleRoomColour(shade_b, D_800BD026);
    D_800BEA60[5] = 0;
    D_800BEA60[6] = Render_ScaleRoomColour(shade_a, D_800BD027);
    D_800BEA60[7] = Render_ScaleRoomColour(shade_b, D_800BD027);
    D_800BEA60[8] = 0;

    Render_LoadRoomColorMatrix();
    return 0;
}
