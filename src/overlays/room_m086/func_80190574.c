typedef unsigned short u16;
typedef unsigned int u32;
typedef short s16;
typedef int s32;

typedef struct RoomM086Blob8 {
    char bytes[8];
} RoomM086Blob8;

typedef struct RoomM086Ent {
    u16 x;
    u16 y;
    u16 z;
    u16 flag;
    s16 vx;
    s16 vy;
    s16 vz;
    s16 padE;
    s16 timer;
    s16 state;
} RoomM086Ent;

extern void *D_800F32D0;
extern void *D_800F33E0;
extern s16 D_800942EC;
extern s32 D_800E27EC;
extern u16 D_800E11EA;
extern u16 D_800E2850[];
extern s16 D_800F3368;
extern s16 D_800F336A;
extern u16 D_800F336C;
extern s16 D_800F336E;
extern u16 D_800F3370;
extern s16 D_800F3372;
extern s16 D_800F3374;
extern s16 D_800F3376;
extern s16 D_800F3378;
extern char D_8018EFF4[];

void func_800CE560(void *arg0, s32 arg1, s32 arg2, void *arg3);
void *func_800CE610(void *arg0);
void func_800CE8F0(void *arg0, s32 arg1, void *arg2, void *arg3);
s32 func_80071A54(void);
s32 func_80077CF4(s32 arg0);
s32 func_80077DC4(s32 arg0);
s32 func_800D3FD8(void);
void func_800D3F64(s32 arg0, s32 arg1);

extern s32 func_801900CC(s32 arg0, RoomM086Ent *arg1, void *arg2);

#define COPY_POS(dst, src) \
    (dst)->x = (src)->x;   \
    (dst)->y = (src)->y;   \
    (dst)->z = (src)->z

s32 func_80190574(s32 mode, RoomM086Ent *ent) {
    RoomM086Blob8 blob;
    RoomM086Ent *actor;
    s32 phase;
    s32 angle;
    s32 mag;
    s32 i;
    s32 value;

    blob = *(RoomM086Blob8 *)D_8018EFF4;

    if (mode == 1) {
        goto mode1;
    }
    if (mode < 2) {
        if (mode == 0) {
            goto mode0;
        }
        goto ret0;
    }
    if (mode == 2) {
        goto mode2;
    }
    goto ret0;

mode0:
    func_800CE8F0(((void **)D_800F32D0)[2], 7, &blob, ent);
    ent->y = D_800942EC;
    value = func_800D3FD8();
    func_800D3F64(0x579, value);
    func_800CE560(((void **)D_800F33E0)[2], 0x14, 0x18, func_801900CC);
    goto done;

mode1:
    value = D_800E27EC;
    if (value == 1) {
        angle = func_80071A54();
        i = 0;
        do {
            actor = func_800CE610(((void **)D_800F33E0)[2]);
            if (actor != 0) {
                COPY_POS(actor, ent);
                mag = (func_80071A54() & 0xF) + 7;
                value = func_80077DC4(angle) * mag;
                if (value < 0) {
                    value += 0xFFF;
                }
                actor->vx = value >> 12;
                value = func_80077CF4(angle) * mag;
                if (value < 0) {
                    value += 0xFFF;
                }
                actor->vz = value >> 12;
                actor->vy = -(func_80071A54() & 0xF) - 0xE;
                actor->state = 0;
                actor->timer = 0;
            }
            i++;
            angle += 0x2AA;
        } while (i < 6);
        value = D_800E27EC;
    }

    value = D_800E27EC - 2;
    if ((u32)value < 3U) {
        angle = func_80071A54();
        i = 0;
        do {
            actor = func_800CE610(((void **)D_800F33E0)[2]);
            if (actor != 0) {
                COPY_POS(actor, ent);
                mag = (func_80071A54() & 0x1F) + 0x18;
                value = func_80077DC4(angle) * mag;
                if (value < 0) {
                    value += 0xFFF;
                }
                actor->vx = value >> 12;
                value = func_80077CF4(angle) * mag;
                if (value < 0) {
                    value += 0xFFF;
                }
                actor->vz = value >> 12;
                actor->vy = -(func_80071A54() & 0x1F) - 0x18;
                actor->flag = func_80071A54() & 1;
                actor->state = 1;
                actor->timer = 0;
            }
            i++;
            angle += 0xAAA;
        } while (i < 2);
    }

    value = D_800E27EC - 2;
    if ((u32)value < 0x15U) {
        angle = func_80071A54();
        actor = func_800CE610(((void **)D_800F33E0)[2]);
        if (actor != 0) {
            COPY_POS(actor, ent);
            value = func_80071A54();
            phase = actor->x;
            phase -= 0x80;
            phase += value & 0xFF;
            actor->x = phase;
            value = func_80071A54();
            phase = actor->z;
            phase -= 0x80;
            phase += value & 0xFF;
            actor->z = phase;
            mag = (func_80071A54() & 0xF) + 7;
            if (D_800E27EC & 1) {
                actor->vy = -(func_80071A54() & 0x1F) - 0x1E;
            } else {
                actor->vy = -(func_80071A54() & 0xF) - 8;
                mag = (s32)(mag + ((u32)mag >> 31)) >> 1;
            }
            value = func_80077DC4(angle) * mag;
            if (value < 0) {
                value += 0xFFF;
            }
            actor->vx = value >> 12;
            value = func_80077CF4(angle) * mag;
            if (value < 0) {
                value += 0xFFF;
            }
            actor->vz = value >> 12;
            actor->flag = func_80071A54() & 1;
            actor->state = 0;
            actor->timer = 0;
        }
    }
    if (D_800E27EC < 0x46) {
        goto ret0;
    }
    return 1;

mode2:
    D_800F3374 = 0x10;
    D_800F3368 = 0x20;
    D_800F336A = 2;
    D_800F3376 = 0x20;
    D_800F3378 = 0x20;
    D_800F336C = 3;
    D_800F336E = 0;
    D_800F3372 = 0;
    D_800F3370 = D_800E2850[D_800E11EA];

ret0:
    return 0;
done:;
}
