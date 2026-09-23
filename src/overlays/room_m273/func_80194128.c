#include "room_m273.h"
#include "pe1/psyq_gpu.h"

typedef struct {
    s32 reserved[5];
    s32 xyz[3];
} RoomM273PlayerTransform;

typedef struct {
    char reserved[0x238];
    RoomM273PlayerTransform *volatile transform;
} RoomM273Player;

extern RoomM273Player *g_PlayerEntity;

int func_80194128(int mode, GteRotation *rotation) {
    GteShortVector position;
    int frame, size, kind, palette;
    u16 clut;
    register int special asm("$3");

    if (mode == 1) {
        if (D_800E27EC >= 16) return 1;
    } else if (mode == 2) {
        RoomM273Player *player;
        /* The loop index is reused for the sampled scale below. */
        size = 0;
        asm volatile("" : : "r"(size));
        player = g_PlayerEntity;
        for (; size < 3; size++) {
            RoomM273PlayerTransform *transform = player->transform;
            ((s16 *)&position)[size] = transform->xyz[size];
        }

        frame = D_800E27EC - 1;
        kind = D_800F336C;
        size = *(s16 *)((char *)D_800966EC + ((frame << 8) & 0x3F00));
        palette = D_800E1204[kind];
        special = 4;
        if (kind == special && D_800F3428) palette += 11;
        else palette += 7;

        clut = GetClut(0, palette);
        func_800CEE20(&position, rotation, 8192, size * 3, 5, clut, 1,
                       /* The upper half of the packed trig entry is signed. */
                       (s16)(*(s32 *)((char *)D_800966EC +
                           ((frame << 8) & 0x3F00)) >> 16) >> 5, 0);
    }
    return 0;
}
