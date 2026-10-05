#include "common.h"
#include "pe1/boot_stack.h"
#include "pe1/game_state.h"
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

extern u32 g_SceneDispatchToken;
extern u32 g_SceneDispatchCur;
extern u32 g_PlayTimeFrameCounter;

#define STATE_FLAG_IMAGE_BUSY 0x00100000U
#define STATE_FLAG_READY 0x00000100U

void main(void) {
    int mode;
    u32 token;
    Pe1GameState *state;

    InitSystem();
    mode = 0;

    for (;;) {
        state = &g_GameState;
        Sys_SyncShutdown();

        while (OpenPeImage() != 0) {
            VSync(0);
        }

        Boot_InitGameState();
        Boot_InitSubsystems();
        Scene_LoadSceneData();

        g_SceneDispatchToken = 0xA9400048;

        do {
            if (g_GameState.flags & STATE_FLAG_IMAGE_BUSY) {
                Render_InitDisplayLists(mode);
                g_GameState.flags &= ~STATE_FLAG_IMAGE_BUSY;
            }

            Scene_LoadFieldBg();
            token = g_SceneDispatchToken;
            g_SceneDispatchCur = token;

            switch (token) {
            case 0xA8000048:
                Overlay_LoadTables();
                BOOT_CALL_ON_SCRATCHPAD_STACK(BOOT_SCRATCHPAD_STACK_TOP, func_8019234C());
                g_GameState.flags |= 1;
                break;
            case 0xAA108448:
                if ((g_GameState.display_list_modes & 2) == 0) {
                    mode = 2;
                    g_GameState.flags |= STATE_FLAG_IMAGE_BUSY;
                    break;
                }
                CD_LoadBootAudio();
                func_801235DC();
                g_SceneDispatchToken = 0xA80830C8;
                g_GameState.flags |= 1;
                break;
            case 0xA9400048:
                Overlay_LoadInitialImage();
                Gpu_InitDisplay(func_801909B4());
                g_GameState.flags |= 3;
                break;
            default:
                Boot_RunFrame();
                break;
            }

            if (g_SceneDispatchToken != 0xA80651C8 && g_SceneDispatchToken != 0xA8065248 &&
                g_SceneDispatchToken != 0xA80652C8 && g_SceneDispatchToken != 0xA80660C8 &&
                g_SceneDispatchToken != 0xA8066148 && g_SceneDispatchToken != 0xA80661C8 &&
                g_SceneDispatchToken != 0xA8066348) {
                if (g_PlayTimeFrameCounter < 600) {
                    if ((g_GameState.display_list_modes & 1) == 0) {
                        mode = 1;
                        g_GameState.flags |= STATE_FLAG_IMAGE_BUSY;
                    }
                } else if ((g_GameState.display_list_modes & 2) == 0) {
                    mode = 2;
                    g_GameState.flags |= STATE_FLAG_IMAGE_BUSY;
                }
            }
        } while ((state->flags & STATE_FLAG_READY) == 0);

        VSync(0);
        SetDispMask(0);
        state->flags &= ~STATE_FLAG_READY;
    }
}
