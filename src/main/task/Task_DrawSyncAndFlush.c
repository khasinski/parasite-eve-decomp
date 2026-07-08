/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#include "pe1/reloc_block.h"

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
    count = (unsigned char)block->u0.base[0];
    if (count != 0) {
        i = 0;
        offset = 1;
        do {
            Scene_LoadMap(block->u0.base + offset, 0, 1);
            block = g_SceneDataTable1;
            i++;
            offset += 2;
        } while (i < (unsigned char)block->u0.base[0]);
    }
}
