/* CC1_FLAGS: -G8 -g3 */

int Geo_PointInPoly(int arg0, int arg1, int *arg2, int arg3);

typedef signed char s8;
typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;
typedef int s32;

void EnterCriticalSection(void);
void ExitCriticalSection(void);
void FlushCache(void);
void VSync(s32);
s32 CdRom_ReadSectorsFromLba(s32, s32, s32);
s32 CdRom_PollReady(void);
void Render_InitEntityPool(s32);
void SetDispMask(s32);
void DrawSync(s32);
void func_801216C4(s32, s32 *);
void func_80121C04(s32);
void func_801223A8(s32);

extern s32 D_8001160C[];
#define D_8001160C (D_8001160C[0])
extern s32 D_80011610[];
#define D_80011610 (D_80011610[0])
extern u16 g_SceneInitLbaTbl0[];
extern u16 g_SceneInitLbaTbl1[];
extern u16 g_SceneAssetLbaTbl[];
#define g_SceneAssetLbaTbl (g_SceneAssetLbaTbl[0])
extern s32 g_GameState[];

int Task_PointInPoly(int **arg0) {
    int local[8];
    unsigned int i;

    for (i = 0; i < 4; i++) {
        local[i * 2] = *arg0[i * 2];
        local[i * 2 + 1] = *arg0[i * 2 + 1];
    }

    *arg0[10] = Geo_PointInPoly(*arg0[8], *arg0[9], local, 4);
    return 1;
}

s32 Task_LoadSceneData(s16 **arg0) {
    s16 **arg0_reg = arg0;
    s32 *base = g_GameState;
    s32 sp[2];
    s32 ret;

    base[0] |= 0x8200;
    DrawSync(0);
    VSync(0);
    SetDispMask(0);
    Render_InitEntityPool(1);

retry_first:
    {
        u16 *range = g_SceneInitLbaTbl0;
        s32 minus_one = -1;
        do {
            ret = CdRom_ReadSectorsFromLba(base[0x40] + range[0], D_8001160C, range[1] - range[0]);
        } while (ret == minus_one);
    }
    {
        s32 wait = -1;
        while (1) {
            ret = CdRom_PollReady();
            if (ret == 0) {
                break;
            }
            if (ret == wait) {
                goto retry_first;
            }
        }
    }

    EnterCriticalSection();
    FlushCache();
    ExitCriticalSection();

retry_second:
    {
        u16 *range = g_SceneInitLbaTbl1;
        s32 minus_one = -1;
        do {
            ret = CdRom_ReadSectorsFromLba(base[0x40] + range[0], D_80011610, range[1] - range[0]);
        } while (ret == minus_one);
    }
    {
        s32 wait = -1;
        while (1) {
            ret = CdRom_PollReady();
            if (ret == 0) {
                break;
            }
            if (ret == wait) {
                goto retry_second;
            }
        }
    }

    EnterCriticalSection();
    FlushCache();
    ExitCriticalSection();

    sp[1] = 0;
    sp[0] = D_80011610 + ((g_SceneAssetLbaTbl - g_SceneInitLbaTbl1[0]) << 0xB);
    func_801216C4(1, sp);
    func_80121C04(**arg0_reg);
    func_801223A8(1);
    return 1;
}
