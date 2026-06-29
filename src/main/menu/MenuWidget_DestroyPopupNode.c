void MenuWidget_DestroyNodeRecursive(void *node);

void MenuWidget_DestroyPopupNode(void *node) {
    void *owner;

    owner = *(void **)((char *)node + 0x34);
    *(int *)((char *)owner + 0x80) = 0;
    MenuWidget_DestroyNodeRecursive(node);
}
