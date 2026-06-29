/* CC1_PROFILE: build0001 */
/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

extern char g_EntityAllocBlockTable[];

void Entity_FreeAllocationBlock(int arg0) {
    int i;
    int offset;

    i = 0;
    while ((unsigned int)(i & 0xFF) < 0x10U) {
        offset = (i & 0xFF) << 3;
        if (*(int *)(g_EntityAllocBlockTable + offset) == arg0) {
            *(int *)(g_EntityAllocBlockTable + offset) = 0;
            return;
        }
        i++;
    }
}
