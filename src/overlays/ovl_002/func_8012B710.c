typedef struct Overlay002ListNode {
    struct Overlay002ListNode *next;
    char pad4[0x28];
    int kind;
} Overlay002ListNode;

extern Overlay002ListNode *D_801D1370;
extern int D_80193200;

void func_8012B710(void) {
    Overlay002ListNode *node;
    int kind;

    node = D_801D1370;
    kind = 2;
    while (node != 0) {
        if (node->kind == kind) {
            break;
        }
        node = node->next;
    }

    *(void **)((char *)node + 0xC) = &D_80193200;
}
