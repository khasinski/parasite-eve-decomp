/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: --use-comm-section -G8 */

void MenuWidget_DestroyNodeRecursive(void *node);

void MenuWidget_DestroyPopupNode(void *node) {
    void *owner = *(void **)((char *)node + 0x34);

    *(int *)((char *)owner + 0x80) = 0;
    MenuWidget_DestroyNodeRecursive(node);
}
