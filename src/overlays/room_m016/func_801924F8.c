typedef unsigned short u16;
typedef short s16;
typedef int s32;

typedef struct RoomM298FxRec {
    s16 x;
    s16 y;
    s16 z;
    s16 pad6;
    s32 radius;
} RoomM298FxRec;

extern s32 D_800E27EC;
extern u16 D_800E1204[];
extern s16 D_800F336A;
extern u16 D_800F336C;
extern s32 D_800F3428;

s32 func_80071A54(void);
s32 func_80077AA4(s32 arg0, s32 arg1);
s32 func_80077DC4(s32 arg0);
s32 func_800CEE20(RoomM298FxRec *rec, void *arg1, s32 arg2, s32 arg3, s32 arg4, s32 arg5, s32 arg6, s32 arg7, s32 arg8);

s32 func_801924F8(s32 mode, RoomM298FxRec *rec) {
    s32 value;
    s32 scale;
    s32 tex;
    s32 fade;

    if (mode == 1) {
        goto mode1;
    }
    if (mode == 2) {
        goto mode2;
    }
    return 0;

mode1:
    rec->y -= 2;
    rec->x += (func_80071A54() & 7) - 3;
    rec->z += (func_80071A54() & 7) - 3;
    if (D_800E27EC < 0xE) {
        goto ret0;
    }
    return 1;

mode2:
    {
        /* Keep the divider magic live in $s0; otherwise allocation shifts. */
        register s32 magic asm("$16") = 0x92492493;

        fade = func_80077DC4((D_800E27EC << 10) / 14) / 50;
        value = ((func_80077DC4((D_800E27EC << 10) / 14) / 2) + 0x800) * rec->radius;
        if (value < 0) {
            value += 0xFFF;
        }
        scale = (value >> 12) + 0x400;
        tex = D_800E1204[D_800F336C];
        if (D_800F336C == 4 && D_800F3428 != 0) {
            tex += 4;
        }
        func_800CEE20(rec, 0, scale, scale, (D_800F336A * ((D_800E27EC << 3) / 14)) + 0xE0,
                      (u16)func_80077AA4(0x40, tex), 1, fade, 0);
    }

ret0:
    return 0;
}
