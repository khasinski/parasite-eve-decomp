/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 */

typedef struct RelocBlock {
    char *base;
} RelocBlock;

extern RelocBlock *g_SceneDataTable1;

int DrawSync(int arg0);
void Scene_LoadMap();

void Task_DrawSyncAndFlush(void) {
    unsigned int i;
    register int offset asm("$17");
    RelocBlock *block;
    unsigned int count;
    char frame[8];

    DrawSync(0);
    block = g_SceneDataTable1;
    count = (unsigned char)block->base[0];
    if (count != 0) {
        i = 0;
        offset = 1;
        do {
            Scene_LoadMap(block->base + offset, 0, 1);
            block = g_SceneDataTable1;
            i++;
            offset += 2;
        } while (i < (unsigned char)block->base[0]);
    }
}
