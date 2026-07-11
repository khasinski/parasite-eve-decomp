typedef signed short s16;
typedef unsigned short u16;
typedef unsigned int u32;

typedef struct GeoPointEntry {
    u32 flags;
    char pad_04[4];
    u16 base_x;
    u16 base_y;
    s16 x;
    s16 y;
    s16 min_x;
    s16 max_x;
    s16 min_y;
    s16 max_y;
} GeoPointEntry;

int Geo_TransformPoint(GeoPointEntry *entry, int dx, int dy, int dz) {
    int x;
    int y;
    int z;

    x = (s16)(entry->base_x + dx);
    y = (s16)(entry->base_y + dy);
    z = dz + (entry->flags >> 20);

    if (x < entry->min_x) {
        x = entry->min_x;
    } else if (entry->max_x < x) {
        x = entry->max_x;
    }

    if (y < entry->min_y) {
        y = entry->min_y;
    } else if (entry->max_y < y) {
        y = entry->max_y;
    }

    entry->x = x;
    entry->y = y;
    entry->flags = (entry->flags & 0xFFF000FF) | ((z & 0xFFF) << 8);

    return 0;
}
