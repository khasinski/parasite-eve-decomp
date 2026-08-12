#include "pe1/geom_state.h"

int Geo_TransformPoint(GeomEntry *entry, int x, int y, int depth) {
    int screenX;
    int screenY;

    screenX = entry->base_x + x;
    depth += (*(unsigned int *)entry) >> 20;
    if ((short)screenX < entry->ot10) {
        screenX = entry->ot10;
    } else if (entry->ot12 < (short)screenX) {
        screenX = entry->ot12;
    }

    screenY = entry->base_y + y;
    if ((short)screenY < entry->ot14) {
        screenY = entry->ot14;
    } else if (entry->ot16 < (short)screenY) {
        screenY = entry->ot16;
    }

    entry->scr_x = screenX;
    entry->scr_y = screenY;
    *(unsigned int *)entry = (*(unsigned int *)entry & 0xfff000ff) |
                             ((depth & 0xfff) << 8);
    return 0;
}
