typedef signed short s16;
typedef signed int s32;
typedef unsigned int u32;

typedef struct SceneE04VecEntry {
    s16 x;
    s16 y;
    s16 z;
    char pad6[2];
} SceneE04VecEntry;

typedef struct SceneE04VecTable {
    SceneE04VecEntry entries[16];
    s16 scale;
    s16 count;
} SceneE04VecTable;

void *func_800C2B50(void);

void func_8018FE7C(void *arg0, void *arg1, SceneE04VecTable *out) {
    char *slot;
    u32 i;

    slot = func_800C2B50();
    for (i = 0; i < 16; i++) {
        out->entries[i].x = *(s32 *)(slot + 0x18);
        out->entries[i].y = *(s32 *)(slot + 0x1C);
        out->entries[i].z = *(s32 *)(slot + 0x20);
    }
    out->count = 0x80;
    out->scale = 0x800;
}
