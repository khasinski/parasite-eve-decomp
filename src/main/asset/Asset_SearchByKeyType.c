/* CC1_FLAGS: -g3 */
/* MASPSX_FLAGS: --use-comm-section */

typedef unsigned char u8;
typedef int s32;
typedef unsigned int u32;

extern void *g_LoadedSceneAssetBlock[];
int Asset_FindTable08ByU32Key(void *arg0, s32 arg1);

int Asset_SearchByKeyType(int arg0) {
    u32 key;
    u32 mask;
    int wanted;
    register void *base asm("$19");
    u32 type_mask;
    u32 high_mask;
    u32 high_value;
    u8 *entry;
    u32 value;

    wanted = arg0;
    key = 0x73DECD80;
    mask = 0x1FFC00;
    base = g_LoadedSceneAssetBlock[0];
    type_mask = 0xD0000;
    high_mask = 0xFFE00000;
    high_value = 0x20000000;

    while (1) {
        entry = (u8 *)Asset_FindTable08ByU32Key(base, key);
        if (entry == 0) {
            return -1;
        }

        if (entry[3] == wanted) {
            value = *(u32 *)(entry + 8);
            if ((value & mask) == type_mask) {
                return ((value & high_mask) ^ high_value) != 0;
            }
        }
        key += 4;
    }
}
