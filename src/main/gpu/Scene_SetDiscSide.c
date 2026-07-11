typedef signed char s8;
typedef unsigned char u8;
typedef unsigned int u32;

extern struct { char _[16]; } D_800B0CD8_o __asm__("D_800B0CD8");
extern struct { char _[16]; } D_8009D1A0_o __asm__("D_8009D1A0");
extern struct { char _[16]; } D_8009D2E8_o __asm__("D_8009D2E8");

#define GAME_STATE ((u8 *)&D_800B0CD8_o)
#define D_8009D1A0 (*(u32 *)&D_8009D1A0_o)
#define D_8009D2E8 (*(u32 *)&D_8009D2E8_o)

#define SCENE_DISC_SIDE_CURRENT GAME_STATE[0x0C]
#define SCENE_DISC_SIDE_PENDING GAME_STATE[0x0D]
#define SCENE_DISC_FLAGS GAME_STATE[0x0E]

int Scene_SetDiscSide(s8 side) {
    u8 flags;

    if (side == -1) {
        side = (s8)SCENE_DISC_SIDE_PENDING;
        SCENE_DISC_SIDE_CURRENT = SCENE_DISC_SIDE_PENDING;
        SCENE_DISC_FLAGS |= 3;
    }

    if (((D_8009D1A0 & 2) != 0) || ((*(u32 *)GAME_STATE & 2) != 0)) {
        SCENE_DISC_FLAGS |= 2;
        D_8009D2E8 &= ~2U;
    }

    flags = SCENE_DISC_FLAGS;
    if ((flags & 4) != 0) {
        SCENE_DISC_FLAGS = (flags | 3) & ~4;
    }

    if ((u32)(side - 1) < 8U) {
        if (side != (s8)SCENE_DISC_SIDE_PENDING) {
            SCENE_DISC_SIDE_PENDING = side;
            SCENE_DISC_SIDE_CURRENT = side;
            SCENE_DISC_FLAGS |= 1;
        }
    }

    return 0;
}
