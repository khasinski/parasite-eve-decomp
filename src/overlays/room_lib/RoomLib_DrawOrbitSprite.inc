extern u8 gOrbitSpriteAlpha __asm__("D_80194354");
extern u16 gOrbitSpriteDepth __asm__("D_8019435A");

char *func_800C2B50(void);
void func_800C2EAC(int arg0);
void func_800C2FF0(int arg0, int arg1);
void func_800C3098(int arg0);
void func_800C3238(int arg0);
void func_80071A44(void *arg0, int arg1, int arg2);
void func_80078CC4(RoomSpriteMatrix *matrix, RoomFxVec4 *scale);
void func_800C42A4(void *arg0, RoomSpriteMatrix *matrix, int arg2);

void func_80190C0C(void *unused, void *unused2, RoomOrbitSpriteFxParams *fx) {
    RoomSpriteMatrix matrix;
    RoomFxVec4 scratchScale;
    RoomFxVec4 scale;
    char *owner;
    u8 *drawSlot;

    owner = func_800C2B50();
    func_800C2EAC(owner[0x24]);
    func_800C2FF0(0x20, 0x20);
    func_800C3098(0x10);
    func_800C3238(2);

    matrix.m[2][2] = 0x1000;
    matrix.m[1][1] = 0x1000;
    matrix.m[0][0] = 0x1000;
    matrix.t[0] = matrix.t[1] = matrix.t[2] = 0;
    matrix.m[0][1] = matrix.m[0][2] = matrix.m[1][0] =
        matrix.m[1][2] = matrix.m[2][0] = matrix.m[2][1] = 0;

    func_80071A44(&scale, 0, 0x10);
    scale.x = fx->scale;
    scale.y = fx->scale;
    scale.z = 0x400;
    scratchScale = scale;
    func_80078CC4(&matrix, &scratchScale);

    drawSlot = &gOrbitSpriteAlpha;
    matrix.t[0] = fx->x;
    matrix.t[1] = fx->y;
    matrix.t[2] = fx->z;
    *drawSlot = ((s16)fx->alpha >> 1) << 1;
    gOrbitSpriteDepth = fx->depth;
    func_800C42A4(drawSlot - 4, &matrix, 1);
}
