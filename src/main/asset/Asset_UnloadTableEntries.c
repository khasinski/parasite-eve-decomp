/* CC1_FLAGS: -fno-force-mem */
#include "pe1/scene_assets.h"

static inline void Unload(int index) {
    SceneAssetHandler *handler = g_PmCmdHandlerTable[index];
    if (handler && handler->unload) {
        handler->unload();
    }
}

int Asset_UnloadTableEntries(void) {
    Pe1GameState *state = &g_GameState;
    int i;

    if (g_GameStateFlags & 0x80) {
        for (i = 0; i < 8; i++) {
            Unload(i);
        }
        Unload(85);
        {
            SceneAssetBlob *blob = state->loaded_scene_assets;
            SceneAssetDirectory *directory =
                (SceneAssetDirectory *)((u8 *)blob + blob->directoryOffset);
            SceneAssetRecord *entry =
                (SceneAssetRecord *)((u8 *)blob + (directory->entries & 0x3FFFFF));

            for (i = 0; i < (int)(directory->entries >> 22); i++) {
                unsigned int id = entry[i].handlerId;
                if (id >= 8 && id < 85) {
                    Unload(id);
                }
            }
        }
        g_GameStateFlags &= ~0x80;
    }
    for (i = 8; i < 85; i++) {
        void **slot = &g_PmCmdHandlerTable[i];
        if (*slot) {
            *slot = 0;
        }
    }
    for (i = 30; (unsigned int)i < 104; i++) {
        if (D_800E1044[i]) {
            D_800E1044[i] = 0;
        }
    }
    return 0;
}
