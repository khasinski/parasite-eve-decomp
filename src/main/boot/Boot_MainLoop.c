/* CC1_FLAGS: -g3 */

typedef unsigned char u8;
typedef unsigned int u32;


extern void InitSystem(void);
extern void Sys_SyncShutdown(void);
extern int OpenPeImage(void);
extern int VSync(int arg0);
extern void Boot_InitGameState(void);
extern void Boot_InitSubsystems(void);
extern void Scene_LoadSceneData(void);
extern void Render_InitDisplayLists(int arg0);
extern void Scene_LoadFieldBg(void);
extern void Overlay_LoadTables(void);
extern void func_8019234C(void);
extern void CD_LoadBootAudio(void);
extern void func_801235DC(void);
extern int Overlay_LoadInitialImage(void);
extern int func_801909B4(void);
extern void Gpu_InitDisplay(int arg0);
extern void Boot_RunFrame(void);
extern void SetDispMask(int arg0);

extern u32 g_GameState[];
extern u32 g_SceneDispatchToken;
extern u32 g_SceneDispatchCur;
extern u32 g_PlayTimeFrameCounter;

#define STATE_FLAG_IMAGE_BUSY 0x00100000U
#define STATE_FLAG_READY 0x00000100U
#define LAUNDER(x) asm volatile("" : "=r"(x) : "0"(x))

void main(void) {
    int retryMode;
    register u32 imageMagic asm("$19");
    u32 busyFlag;
    u32 *state;
    u8 *stateByte;
    register u32 value asm("$2");
    u32 dispatchValue;

    InitSystem();

    retryMode = 0;
    imageMagic = 0xA9400048;
    LAUNDER(imageMagic);
    busyFlag = STATE_FLAG_IMAGE_BUSY;
    state = g_GameState;
    stateByte = (u8 *)state + 0xF5;

restart:
    Sys_SyncShutdown();

    while (OpenPeImage() != 0) {
        VSync(0);
    }

    Boot_InitGameState();
    Boot_InitSubsystems();
    Scene_LoadSceneData();

    g_SceneDispatchToken = imageMagic;

loop:
    if (state[0] & busyFlag) {
        Render_InitDisplayLists(retryMode);
        state[0] &= 0xFFEFFFFF;
    }

    Scene_LoadFieldBg();
    dispatchValue = g_SceneDispatchToken;
    g_SceneDispatchCur = dispatchValue;

    if (dispatchValue == imageMagic) {
        goto case_image_magic;
    }
    if (imageMagic < dispatchValue) {
        goto check_high_range;
    }
    if (dispatchValue == 0xA8000048) {
        goto case_a8000048;
    }
    goto default_case;

check_high_range:
    if (dispatchValue == 0xAA108448) {
        goto case_aa108448;
    }
    goto default_case;

case_a8000048:
    Overlay_LoadTables();
    asm volatile(
        "lui $5,0x1f80\n"
        "ori $5,$5,0x3fc\n"
        "addu $8,$5,$0\n"
        "sw $29,0($8)\n"
        "addiu $8,$8,-4\n"
        "addu $29,$8,$0"
    );
    func_8019234C();
    asm volatile(
        "addiu $29,$29,4\n"
        "lw $29,0($29)"
    );
    value = state[0];
    goto set_one;

case_aa108448:
    if ((*((u8 *)state + 0xF5) & 2) == 0) {
        retryMode = 2;
        state[0] |= busyFlag;
        goto after_dispatch;
    } else {
        CD_LoadBootAudio();
        func_801235DC();
        value = state[0];
        g_SceneDispatchToken = 0xA80830C8;
    }

set_one:
    value |= 1;
    state[0] = value;
    goto after_dispatch;

case_image_magic:
    Overlay_LoadInitialImage();
    Gpu_InitDisplay(func_801909B4());
    state[0] |= 3;
    goto after_dispatch;

default_case:
    Boot_RunFrame();

after_dispatch:
    {
        u32 postValue;

        postValue = g_SceneDispatchToken;
        if (postValue != 0xA80651C8 &&
            postValue != 0xA8065248 &&
            postValue != 0xA80652C8 &&
            postValue != 0xA80660C8 &&
            postValue != 0xA8066148 &&
            postValue != 0xA80661C8 &&
            postValue != 0xA8066348) {
            if (g_PlayTimeFrameCounter < 600) {
                if ((*stateByte & 1) == 0) {
                    retryMode = 1;
                    *(u32 *)(stateByte - 0xF5) |= busyFlag;
                }
            } else if ((*stateByte & 2) == 0) {
                retryMode = 2;
                *(u32 *)(stateByte - 0xF5) |= busyFlag;
            }
        }
    }

    if ((state[0] & STATE_FLAG_READY) == 0) {
        goto loop;
    }

    VSync(0);
    SetDispMask(0);
    state[0] &= ~STATE_FLAG_READY;
    goto restart;
}
