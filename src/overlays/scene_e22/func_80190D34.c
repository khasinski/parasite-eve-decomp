#include "scene_e22_shared.h"

typedef SceneE22CallbackRecord OverlayCallback;

s32 func_80190D34(OverlayCallback *arg0) {
    arg0->callback(arg0);
    return 0;
}
