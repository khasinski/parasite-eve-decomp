/* CC1_FLAGS: -g3 -G8 */
/* MASPSX_FLAGS: -G8 */
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
s32 func_8006E7E8(void);
void Render_InitEntityPool(s32);
void func_80074D28(s32);
void func_80074DC0(s32);
void func_801216C4(s32, s32 *);
void func_80121C04(s32);
void func_801223A8(s32);

extern struct { char _[16]; } D_8001160C_o __asm__("D_8001160C");
#define D_8001160C (*(s32 *)&D_8001160C_o)
extern struct { char _[16]; } D_80011610_o __asm__("D_80011610");
#define D_80011610 (*(s32 *)&D_80011610_o)
extern u16 g_SceneInitLbaTbl0[];
extern u16 g_SceneInitLbaTbl1[];
extern struct { char _[16]; } D_80093162_o __asm__("g_SceneAssetLbaTbl");
#define g_SceneAssetLbaTbl (*(u16 *)&D_80093162_o)
extern s32 g_GameState[];

s32 Task_LoadSceneData(s16 **arg0) {
    s16 **arg0_reg = arg0;
    s32 *base = g_GameState;
    s32 sp[2];
    s32 ret;

    base[0] |= 0x8200;
    func_80074DC0(0);
    VSync(0);
    func_80074D28(0);
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
            ret = func_8006E7E8();
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
            ret = func_8006E7E8();
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
