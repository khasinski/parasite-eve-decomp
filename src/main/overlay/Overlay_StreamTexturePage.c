/* CC1_FLAGS: -g3 */

typedef signed char s8;
typedef unsigned short u16;
typedef int s32;

s32 DrawSync(s32 arg0);
s32 CdRom_ReadSectorsFromLba(s32 arg0, s32 arg1, s32 arg2);
s32 CdRom_PollReady(void);
s32 Gpu_LoadTimImage(s32 arg0);

extern s8 D_800B0CE0;
extern s8 g_LoadedTexturePageId;
extern s8 g_GameState[];
extern u16 g_EntityTexLbaStartTbl[];

s32 Overlay_StreamTexturePage(void) {
    s8 *state;
    register u16 *ranges asm("$17");
    s32 retry;
    u16 *next_ranges;
    s32 offset;
    u16 start;
    s32 poll;

    state = g_GameState;
    if (D_800B0CE0 != g_LoadedTexturePageId) {
        DrawSync(0);
restart:
        ranges = g_EntityTexLbaStartTbl;
        next_ranges = ranges + 1;
        retry = -1;
        do {
            offset = (state[8] + 0x2B) * 2;
            start = *(u16 *)(offset + (s32)ranges);
        } while (CdRom_ReadSectorsFromLba(*(s32 *)(state + 0x100) + start, *(s32 *)(state + 0x194), *(u16 *)(offset + (s32)next_ranges) - start) == retry);

        ranges = (u16 *)-1;
        do {
            poll = CdRom_PollReady();
            if (poll == 0) {
                break;
            }
            if (poll == (s32)ranges) {
                goto restart;
            }
        } while (1);

        Gpu_LoadTimImage(*(s32 *)(state + 0x194));
        state[9] = state[8];
    }

    return 0;
}
