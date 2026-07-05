typedef unsigned short u16;
typedef unsigned char u8;
typedef int s32;

typedef struct PackedRect3 {
    u16 x;
    u16 y;
    u16 z;
    s32 out1;
    s32 out2;
} PackedRect3;

extern void Akao_Calc3DPan(u16 *rect, s32 *out1, s32 *out2);
extern s32 Akao_Cmd_24(s32 arg0, s32 arg1, s32 arg2, s32 arg3);
extern u8 D_800B0CE8[];

s32 Akao_SetPos3D(s32 arg0, s32 arg1, u16 arg2, u16 arg3, u16 arg4) {
    PackedRect3 local;
    s32 ret = 0;

    local.x = arg2;
    local.y = arg3;
    local.z = arg4;
    Akao_Calc3DPan(&local.x, &local.out1, &local.out2);

    {
        u8 *base = D_800B0CE8;

        if (base[0] != 0) {
            s32 *table = (s32 *)(base + 0x124);

            ret = Akao_Cmd_24(table[arg0], arg1, local.out1, local.out2);
        }
    }

    return ret;
}
