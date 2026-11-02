#include "common.h"
#include "pe1/boot.h"
#include "pe1/boot_disc_check.h"
#include "pe1/boot_services.h"
#include "pe1/boot_stack.h"
#include "pe1/field_bg_load.h"
#include "pe1/game_state.h"
#include "pe1/game_timers.h"
#include "pe1/psyq_gpu.h"
#include "pe1/scene_dispatch.h"

#define STATE_FLAG_IMAGE_BUSY 0x00100000U
#define STATE_FLAG_READY 0x00000100U

void main(void) {
    int mode;
    u32 token;
    Pe1GameState *state;

    Boot_InitSystem();
    mode = 0;

    for (;;) {
        state = &g_GameState;
        Boot_SyncShutdown();

        while (Asset_OpenPeImage() != 0) {
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
                Boot_LoadOverlayTables();
                BOOT_CALL_ON_SCRATCHPAD_STACK(BOOT_SCRATCHPAD_STACK_TOP, func_8019234C());
                g_GameState.flags |= 1;
                break;
            case 0xAA108448:
                if ((g_GameState.display_list_modes & 2) == 0) {
                    mode = 2;
                    g_GameState.flags |= STATE_FLAG_IMAGE_BUSY;
                    break;
                }
                CdRom_LoadBootAudio();
                func_801235DC();
                g_SceneDispatchToken = 0xA80830C8;
                g_GameState.flags |= 1;
                break;
            case 0xA9400048:
                CdRom_LoadInitialImage();
                Draw_InitDisplay(func_801909B4());
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
