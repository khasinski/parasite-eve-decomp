typedef signed char s8;
typedef unsigned char u8;
typedef unsigned int u32;
typedef int s32;

extern struct { char _[16]; } D_800B0CD8_o __asm__("D_800B0CD8");
extern struct { char _[16]; } D_8009D1A0_o __asm__("D_8009D1A0");
extern u32 D_8009D2E8;
extern u8 D_800B0CE4;
extern u8 D_800B0CE5;
extern u8 D_800B0CE6;

#define GAME_STATE ((u8 *)&D_800B0CD8_o)
#define D_8009D1A0 (*(u32 *)&D_8009D1A0_o)

#define SCENE_STORY_DAY_CURRENT GAME_STATE[0x0C]
#define SCENE_STORY_DAY_PENDING GAME_STATE[0x0D]
#define SCENE_STORY_DAY_FLAGS GAME_STATE[0x0E]

int Scene_SetDiscSide(s32 side) {
    register u8 *gameState asm("$6") = GAME_STATE;
    u8 flags;

    if (side == -1) {
        register u8 pending asm("$4");
        register u8 storyFlags asm("$2");

        pending = D_800B0CE5;
        storyFlags = D_800B0CE6;
        side = (s8)pending;
        D_800B0CE4 = pending;
        D_800B0CE6 = storyFlags | 3;
    }

    if (((D_8009D1A0 & 2) != 0) || ((*(u32 *)gameState & 2) != 0)) {
        D_800B0CE6 |= 2;
        D_8009D2E8 &= ~2U;
    }

    flags = gameState[0x0E];
    if ((flags & 4) != 0) {
        gameState[0x0E] = (flags | 3) & ~4;
    }

    if ((u32)(side - 1) < 8U) {
        if (side != (s8)gameState[0x0D]) {
            register s32 newSide asm("$3");

            newSide = side;
            gameState[0x0D] = newSide;
            gameState[0x0C] = newSide;
            gameState[0x0E] |= 1;
        }
    }

    return 0;
}
