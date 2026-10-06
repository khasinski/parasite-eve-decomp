#include "common.h"
/* MASPSX_FLAGS: -G8 --use-comm-section */

/* Active menu window mode and its on-screen bounds.
 * g_MenuActiveMode is private state of this pair. */

u8 g_MenuActiveMode;
volatile u16 D_8009CE84;
u16 g_MenuActiveWindowYOffset;

typedef struct {
    u16 values[8];
} SaveWindowModeBounds;

extern SaveWindowModeBounds D_80010E38;

void Window_SetBoundsByMode(int mode) {
    SaveWindowModeBounds bounds;
    volatile u16 *entry;

    bounds = D_80010E38;
    entry = &bounds.values[(u8)mode * 2];
    D_8009CE84 = entry[0];
    g_MenuActiveMode = mode;
    g_MenuActiveWindowYOffset = entry[1];
}

unsigned int Menu_GetActiveMode(void) {
    return g_MenuActiveMode;
}
